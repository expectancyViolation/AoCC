#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "helpers.h"
#include "two_part_result.h"

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

struct two_part_result *day01(char *buf) {
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
