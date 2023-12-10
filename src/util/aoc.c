#ifndef AOCC_AOCC_H
#define AOCC_AOCC_H
#include "aoc.h"

#include "helpers.h"
#include "ll_tuple.h"
#include "result_db.h"
#include "timer.h"

#define _GNU_SOURCE
#include <curl/curl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO: make this env variable
#define SESSION_FILE "/tmp/session.txt"
#define INPUT_CACHE_DIR "/tmp/aoc"

#define SEP "\t" /* Tab separates the fields */


char *get_input_file_path(int year, int day) {
  char *res;
  asprintf(&res, "%s/%d/day%02d_input.txt", INPUT_CACHE_DIR, year, day);
  return res;
}
void print_aoc_day_status(const struct aoc_day_status *status) {
  printf("day status:\n"
         "\tpart1:%20s (%d)\n"
         "\tpart2:%20s (%d)\n",
         status->part1_status.part_solution, status->part1_status.part_solved,
         status->part2_status.part_solution, status->part1_status.part_solved);
}

void print_aoc_day_task(const struct aoc_day_task *task) {
  printf("task:\n\tyear:\t%30d\n\tday:\t%30d\n\tfile:\t%30s\n", task->year,
         task->day, task->input_file);
}

void print_aoc_day_result(const struct aoc_day_res *result) {
  printf("result:\n\tpart1:\t%30lld\n", result->result.left);
  printf("\tpart2:\t%30lld\n", result->result.right);
  printf("\n");
}

typedef struct aoc_day_res (*aoc_solver)(const struct aoc_day_task task);

struct aoc_benchmark_day {
  struct aoc_day_task task;
  struct aoc_day_res result;
  double solve_duration;
};

void print_day_benchmark(const struct aoc_benchmark_day *res) {
  printf("--------------\n");
  printf("benchmark:\n");
  print_aoc_day_task(&res->task);
  print_aoc_day_result(&res->result);
  printf("took:%f\n\n", res->solve_duration);
}

struct aoc_benchmark_day benchmark_day(aoc_solver fun,
                                       struct aoc_day_task task) {
  struct my_perf_timer *timer = start_perf_measurement();
  const struct aoc_day_res res = fun(task);
  struct aoc_benchmark_day bench_day = {task, res,
                                        stop_perf_measurement(timer)};
  return bench_day;
}

// from https://curl.se/libcurl/c/getinmemory.html
struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}
// end from

// TODO: cache cookie in struct?
void set_session_cookie(CURL *curl, char *session) {
  char my_cookie[1000] = "adventofcode.com" /* Hostname */
      SEP "TRUE"                            /* Include subdomains */
      SEP "/"                               /* Path */
      SEP "FALSE"                           /* Secure */
      SEP "0"       /* Expiry in epoch time format. 0 == Session */
      SEP "session" /* Name */
      SEP;          /* Value */

  strcat(my_cookie, session);
  curl_easy_setopt(curl, CURLOPT_COOKIELIST, my_cookie);
}

// TODO: cache wrong answers?
// TODO: evaluate and cache hints and prevent impossible submissions?
struct aoc_submission_status
submit_answer(int year, int day, enum AOC_DAY_PART part, char *answer) {
  CURL *curl;
  CURLcode curl_res;
  char *session;
  struct aoc_submission_status submission_status;
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  read_file_to_memory(SESSION_FILE, &session, false);
  curl = curl_easy_init();
  if (curl) {
    set_session_cookie(curl, session);
    // URL

    char *post_url = get_answer_url(year, day);
    printf("url: %s\n", post_url);
    curl_easy_setopt(curl, CURLOPT_URL, post_url);
    char *post_fields = format_answer_payload(part, answer);
    printf("fields: %s\n", post_fields);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_res = curl_easy_perform(curl);
    printf("code:%d\n", curl_res);
    if (curl_res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(curl_res));

    bool parse_ok = parse_submission_status(chunk.memory, &submission_status);
    // printf("response:%s\n",chunk.memory);
    curl_easy_cleanup(curl);
    free(post_url);
    free(post_fields);
  }
  free(chunk.memory);
}

// TODO: detect "please don't fetch..."?
void fetch_day_input(int year, int day, char *outfile) {
  CURL *curl;
  CURLcode res;
  char *session;
  read_file_to_memory(SESSION_FILE, &session, false);
  curl = curl_easy_init();
  if (curl) {
    FILE *f = fopen_mkdir(outfile, "w");
    set_session_cookie(curl, session);
    char *post_url = get_input_url(year, day);
    curl_easy_setopt(curl, CURLOPT_URL, post_url);
    free(post_url);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
    res = curl_easy_perform(curl);
    printf("code:%d\n", res);
    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    fclose(f);
    curl_easy_cleanup(curl);
  }
}

void fetch_day_input_cached(int year, int day, char *filepath) {
  printf("%s\n", filepath);
  if (access(filepath, F_OK) != 0) {
    printf("%s not found. fetching...", filepath);
    fetch_day_input(year, day, filepath);
  }
}

struct aoc_day_status fetch_day_status(int year, int day) {
  CURL *curl;
  CURLcode res;
  char *session;
  struct aoc_day_status day_status={};
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  read_file_to_memory(SESSION_FILE, &session, false);
  curl = curl_easy_init();
  if (curl) {
    set_session_cookie(curl, session);
    char *status_url = get_status_url(year, day);
    curl_easy_setopt(curl, CURLOPT_URL, status_url);
    free(status_url);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    res = curl_easy_perform(curl);
    printf("code:%d\n", res);
    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    parse_day_status(chunk.memory,&day_status);


    free(chunk.memory);
    curl_easy_cleanup(curl);
    return day_status;
  }
}

#endif // AOCC_AOCC_H
