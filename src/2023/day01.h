
#ifndef AOCC_DAY01_H
#define AOCC_DAY01_H

#include "../util/aoc.h"
#include "../util/helpers.h"
#include "../util/ll_tuple.h"
#include "../util/parallelize.h"

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

struct ll_tuple day01(char *buf, __attribute__((unused)) long buf_len) {
  struct ll_tuple result={};
  char **buf_pos = &buf;
  while (*buf_pos != NULL) {
    char *line = strsep(buf_pos, "\n");
    result.left +=
        get_first(line, false) * 10 + get_last(line, false);
    result.right += get_first(line, true) * 10 + get_last(line, true);
  }
  return result;
}

void solve_day01() {
  const int year = 2023;
  const int day = 1;

  char *input_buffer;
  const long filesize = get_day_input_cached(year, day, &input_buffer);
  struct ll_tuple day_res = parallelize(day01, ll_tuple_add , input_buffer, filesize,0);
  print_day_result(day, day_res);

  // part 1
  // submit_answer(year, day, day_part_part1, day_res);

  // part 2
  // submit_answer(year, day, day_part_part2, day_res);
  free(input_buffer);
}
#endif // AOCC_DAY01_H
