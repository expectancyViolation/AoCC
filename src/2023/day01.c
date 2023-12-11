#include "day01.h"

#include "../util/parallelize.h"

#include <malloc.h>
#include <stdbool.h>
#include <string.h>

static const char *digit_and_names[20] = {
    "0",    "1",    "2",   "3",     "4",     "5",   "6",
    "7",    "8",    "9",   ".",     "one",   "two", "three",
    "four", "five", "six", "seven", "eight", "nine"};
static const long long masks[20] = {
    255,        255,      255,           255,           255,
    255,        255,      255,           255,           255,
    255,        16777215, 16777215,      1099511627775, 4294967295,
    4294967295, 16777215, 1099511627775, 1099511627775, 4294967295};

// check if "digit" starts at position j in buffer buff_int
static int matches(const long long *buff_int, const int j) {
  const long long *dig_int = (long long *)(digit_and_names[j]);
  const long long difference = (*buff_int) ^ (*dig_int);
  return !((difference) & (masks[j]));
}

// unsafe: we read over the end of the buffer :/
// but: it's fast :D
static int get_first(const char *buffer, bool part2) {
  for (int i = 0; buffer[i] != 0; i++) {
    for (int j = 0; j < (part2 ? 20 : 10); j++) {
      if (matches((long long *)(buffer + i), j)) {
        return j % 10;
      }
    }
  }
  return 0;
}

static int get_last(const char *buffer, bool part2) {
  for (int i = (int)strlen(buffer); i >= 0; i--) {
    for (int j = 0; j < (part2 ? 20 : 10); j++) {
      if (matches((long long *)(buffer + i), j)) {
        return j % 10;
      }
    }
  }
  return 0;
}

LLTuple year23_day01(char *buf, long buf_len) {
  LLTuple result = {};
  char **buf_pos = &buf;
  while (*buf_pos != NULL) {
    char *line = strsep(buf_pos, "\n");
    result.left += get_first(line, false) * 10 + get_last(line, false);
    result.right += get_first(line, true) * 10 + get_last(line, true);
  }
  return result;
}
AocDayRes solve_year23_day01(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  LLTuple res =
      parallelize(year23_day01, ll_tuple_add, input_buffer, filesize, 0);
  AocDayRes day_res = {res};
  free(input_buffer);
  return day_res;
}
