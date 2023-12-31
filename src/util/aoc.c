#ifndef AOCC_AOCC_H
#define AOCC_AOCC_H
#include "aoc.h"

#include "helpers.h"
#include "timer.h"

#include "../../res/asprintf.c/asprintf.h"

#ifdef CURL_AVAILABLE
#include <curl/curl.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

// TODO: make this env variable
#define INPUT_CACHE_DIR "/tmp/aoc"

#define SEP "\t" /* Tab separates the fields */


char *get_input_file_path(int year, int day) {
  char *res;
  asprintf(&res, "%s/%d/day%02d_input.txt", INPUT_CACHE_DIR, year, day);
  return res;
}

AocBenchmarkDay benchmark_day(aoc_solver fun, AocDayTask task) {
  struct my_perf_timer *timer = start_perf_measurement();
  const AocDayRes res = fun(task);
  AocBenchmarkDay bench_day = {task, res, stop_perf_measurement(timer)};
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

char *get_session() { return getenv("AOC_SESSION"); }

// TODO: cache cookie in struct?
#ifdef CURL_AVAILABLE
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
#endif // CURL_AVAILABLE

bool submit_answer(int year, int day, enum AOC_DAY_PART part,
                   const AocPartRes *guess, AocSubmissionStatus *out) {
  bool parse_ok = false;
#ifdef CURL_AVAILABLE
  CURL *curl;
  CURLcode curl_res;
  char *session = get_session();
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  curl = curl_easy_init();
  if (curl) {
    set_session_cookie(curl, session);
    // URL

    char *post_url = get_answer_url(year, day);
    printf("url: %s\n", post_url);
    curl_easy_setopt(curl, CURLOPT_URL, post_url);
    char *post_fields = format_answer_payload(part, guess);
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

    parse_ok = parse_submission_status(chunk.memory, out);
    curl_easy_cleanup(curl);
    free(post_url);
    free(post_fields);
  }
  free(chunk.memory);
#endif // CURL_AVAILABLE
  return parse_ok;
}

// TODO: detect "please don't fetch..."?
void fetch_day_input(int year, int day, char const *outfile) {
#ifdef CURL_AVAILABLE
  CURL *curl;
  CURLcode res;
  char *session = get_session();
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
#endif // CURL_AVAILABLE
}

void fetch_day_input_cached(int year, int day, char const *filepath) {
  if (access(filepath, F_OK) != 0) {
    printf("%s not found. fetching...", filepath);
    fetch_day_input(year, day, filepath);
  }
}

AocDayStatus fetch_day_status(int year, int day) {
#ifdef CURL_AVAILABLE
  CURL *curl;
  CURLcode res;
  char *session = get_session();
  AocDayStatus day_status = {0};
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  curl = curl_easy_init();
  if (curl) {
    set_session_cookie(curl, session);
    char *status_url = get_status_url(year, day);
    printf("STATUS URL:%s\n", status_url);
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

    parse_day_status(chunk.memory, &day_status);

    free(chunk.memory);
    curl_easy_cleanup(curl);
    return day_status;
  }
#endif // CURL_AVAILABLE
  AocDayStatus empty = {0};
  return empty;
}

#endif // AOCC_AOCC_H
