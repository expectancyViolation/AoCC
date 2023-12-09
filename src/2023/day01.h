
#ifndef AOCC_DAY01_H
#define AOCC_DAY01_H

#include "../util/aoc.h"
#include "../util/helpers.h"
#include "../util/parallelize.h"
#include "../util/two_part_result.h"

#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const char *digit_and_names[20] = {"0",    "1",   "2",     "3",     "4",
                                   "5",    "6",   "7",     "8",     "9",
                                   ".",    "one", "two",   "three", "four",
                                   "five", "six", "seven", "eight", "nine"};
const long long masks[20] = {
    255,        255,      255,           255,           255,
    255,        255,      255,           255,           255,
    255,        16777215, 16777215,      1099511627775, 4294967295,
    4294967295, 16777215, 1099511627775, 1099511627775, 4294967295};

// check if "digit" starts at position j in buffer buff_int
int matches(const long long *buff_int, const int j) {
  const long long *dig_int = (long long *)(digit_and_names[j]);
  const long long difference = (*buff_int) ^ (*dig_int);
  return !((difference) & (masks[j]));
}

// unsafe: we read over the end of the buffer :/
// but: it's fast :D
int get_first(const char *buffer, bool part2) {
  for (int i = 0; buffer[i] != 0; i++) {
    for (int j = 0; j < (part2 ? 20 : 10); j++) {
      if (matches((long long *)(buffer + i), j)) {
        return j % 10;
      }
    }
  }
  return 0;
}

int get_last(const char *buffer, bool part2) {
  for (int i = (int)strlen(buffer); i >= 0; i--) {
    for (int j = 0; j < (part2 ? 20 : 10); j++) {
      if (matches((long long *)(buffer + i), j)) {
        return j % 10;
      }
    }
  }
  return 0;
}

struct two_part_result *day01(char *buf, __attribute__((unused)) long buf_len) {
  struct two_part_result *result = allocate_two_part_result();
  char **buf_pos = &buf;
  while (*buf_pos != NULL) {
    char *line = strsep(buf_pos, "\n");
    (result->part1_result) +=
        get_first(line, false) * 10 + get_last(line, false);
    (result->part2_result) += get_first(line, true) * 10 + get_last(line, true);
  }
  return result;
}

void parallel_solve_day01() {
  const int year = 2023;
  const int day = 1;
  struct two_part_result *day_res = allocate_two_part_result();
  char *input_buffer;
  const long filesize = get_day_input_cached(year, day, &input_buffer);
  parallelize((void *(*)(char *, long))(day01),
              (void (*)(void *, void *))(add_consume_partial_result), day_res,
              input_buffer, filesize, 0);
  print_day_result(day, day_res);

  // part 1
  // submit_answer(year, day, day_part_part1, day_res);

  // part 2
  // submit_answer(year, day, day_part_part2, day_res);
  free_two_part_result(day_res);
  free(input_buffer);
}
#endif // AOCC_DAY01_H
