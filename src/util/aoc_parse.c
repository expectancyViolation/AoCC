#include "aoc_parse.h"

#define PUZZLE_ANSWER_SEARCH_STRING "Your puzzle answer was <code>"
#define PUZZLE_ANSWER_NOT_RIGHT "not the right answer"
#define PUZZLE_ANSWER_RIGHT "gold star"
#define PUZZLE_ANSWER_TOO_LOW "too low"
#define PUZZLE_ANSWER_TOO_HIGH "too high"
#define PUZZLE_ANSWER_TOO_RECENTLY "too recently"
#define PUZZLE_ANSWER_WAIT_ONE_MINUTE "one minute"
#define PUZZLE_ANSWER_WAIT_FIVE_MINUTES "5 minutes"
#define PUZZLE_LEFT_TO_WAIT_BEGIN "You have"
#define PUZZLE_LEFT_TO_WAIT_END "left to wait"
#define PUZZLE_ALREADY_COMPLETE "already complete"

#define DEBUG_CACHE_RESPONSES
#define DEBUG_CACHE_RESPONSES_DIR "/tmp/aoc/cache"

#ifdef DEBUG_CACHE_RESPONSES
#include <time.h>
#endif

void cache_response(char *subdir, const char *raw_response) {
  char *outfile;
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);
  asprintf(&outfile, "%s/%s/response_%ld.txt", DEBUG_CACHE_RESPONSES_DIR,
           subdir, end.tv_sec);
  FILE *f = fopen_mkdir(outfile, "w");
  fputs(raw_response, f);
  free(outfile);
}

bool parse_day_status(char *raw_response, struct aoc_day_status *out) {
#ifdef DEBUG_CACHE_RESPONSES
  cache_response("parse", raw_response);
#endif
  char *parse_pos = raw_response;
  for (int i = 0; i < 2; i++) {
    parse_pos = strstr(parse_pos, PUZZLE_ANSWER_SEARCH_STRING);
    if (parse_pos == NULL)
      break;
    const int offset = strlen(PUZZLE_ANSWER_SEARCH_STRING);
    parse_pos += offset;
    char *code_end = strstr(parse_pos, "</code>");
    printf("response: '%.*s'\n", (int)(code_end - parse_pos), parse_pos);
    switch (i) {
    case 0:
      strncpy(out->part1_status.part_solution, parse_pos, code_end - parse_pos);
      out->part1_status.part_solved = true;
      break;
    case 1:
      strncpy(out->part2_status.part_solution, parse_pos, code_end - parse_pos);
      out->part2_status.part_solved = true;
    }
    parse_pos = code_end;
  }
}

bool parse_submission_status(char *raw_response,
                             struct aoc_submission_status *out) {
#ifdef DEBUG_CACHE_RESPONSES
  cache_response("submit", raw_response);
#endif
  char *article_begin = strstr(raw_response, "<article>");

  char *right_answer = strstr(article_begin, PUZZLE_ANSWER_RIGHT);
  const bool not_correct =
      NULL != strstr(article_begin, PUZZLE_ANSWER_NOT_RIGHT);
  const bool correct = NULL != strstr(article_begin, PUZZLE_ANSWER_RIGHT);
  const bool too_low = NULL != strstr(article_begin, PUZZLE_ANSWER_TOO_LOW);
  const bool too_high = NULL != strstr(article_begin, PUZZLE_ANSWER_TOO_HIGH);
  const bool already_complete = NULL != strstr(article_begin, PUZZLE_ALREADY_COMPLETE);
  const bool too_recently =
      NULL != strstr(article_begin, PUZZLE_ANSWER_TOO_RECENTLY);
  out->correct = correct;
  out->was_checked = not_correct || correct;
  out->too_high = too_high;
  out->too_low = too_low;
  out->already_complete=already_complete;
  return false;
}

void test_aoc_parse() {
  char *buff;
  struct aoc_submission_status status = {};
  read_file_to_memory("/tmp/aoc/cache/submit/response_19744.txt", &buff, false);

  parse_submission_status(buff, &status);
}

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
char *get_status_url(int year, int day) {
  char *res;
  asprintf(&res, "https://adventofcode.com/%d/day/%d", year, day);
  return res;
}
char *format_answer_payload(enum AOC_DAY_PART part, char *answer) {
  char *res;
  asprintf(&res, "level=%d&answer=%s", part, answer);
  return res;
}
