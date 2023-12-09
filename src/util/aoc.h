#ifndef AOCC_AOCC_H
#define AOCC_AOCC_H

#include "helpers.h"
#include "ll_tuple.h"

#define _GNU_SOURCE
#include <curl/curl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// TODO: make this env variable
#define SESSION_FILE "/tmp/session.txt"
#define INPUT_CACHE_DIR "/tmp/aoc"

#define SEP "\t" /* Tab separates the fields */

enum DAY_PART { day_part_part1 = 1, day_part_part2 = 2 };

char *get_answer_url(int year, int day) {
  char *res;
  asprintf(&res, "https://adventofcode.com/%d/day/%d/answer", year, day);
  return res;
}

char *get_input_url(int year, int day) {
  char *res;
  asprintf(&res, "https://adventofcode.com/%d/day/%d/input", year, day);
  return res;
}

char *get_input_file_path(int year, int day) {
  char *res;
  asprintf(&res, "%s/%d/day%02d_input.txt", INPUT_CACHE_DIR, year, day);
  return res;
}

char *format_answer_payload(enum DAY_PART part, char *answer) {
  char *res;
  asprintf(&res, "level=%d&answer=%s", part, answer);
  return res;
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
__attribute__((unused)) void submit_answer(int year, int day,
                                           enum DAY_PART part,
                                           const struct ll_tuple *res) {
  char *answer;
  const long long long_answer =
      (part == day_part_part1) ? res->left : res->right;
  asprintf(&answer, "%lld", long_answer);
  CURL *curl;
  CURLcode curl_res;
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  char *session;
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
    // curl_easy_setopt(curl, CURLOPT_URL, "https://adventofcode.com");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_res = curl_easy_perform(curl);
    printf("code:%d\n", curl_res);
    if (curl_res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(curl_res));

    // printf("response:%s\n",chunk.memory);
    char *main_begin = strstr(chunk.memory, "<main>");
    char *main_end = strstr(main_begin, "</main>");
    printf("response: '%.*s'\n", (int)(main_end - main_begin - 6),
           main_begin + 6);
    /* always cleanup */

    curl_easy_cleanup(curl);
    free(post_url);
    free(post_fields);
  }

  free(answer);
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

long get_day_input_cached(int year, int day, char **file_content_out) {
  char *filepath = get_input_file_path(year, day);
  printf("%s\n", filepath);
  if (access(filepath, F_OK) != 0) {
    printf("%s not found. fetching...", filepath);
    fetch_day_input(year, day, filepath);
  }
  const long long input_size =
      read_file_to_memory(filepath, file_content_out, false);
  free(filepath);
  return input_size;
}

#endif // AOCC_AOCC_H
