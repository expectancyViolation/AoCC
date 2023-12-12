#include "day09.h"

#include "../util/parallelize.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

LLTuple get_extrapolated_val(char *line) {
  long long *numbers = NULL;
  char *endpos;
  while (true) {
    const long num = (strtol(line, &endpos, 10));
    if (endpos == line)
      break;
    line = endpos;
    cvector_push_back(numbers, num);
  }
  const long long n = cvector_size(numbers);
  long long coeff = 1;
  LLTuple res = {0, 0};
  for (int i = 0; i < n; i++) {
    const long long x = numbers[i];
    res.left += coeff * x;
    coeff = (-coeff * (n - i)) / (i + 1);
    res.right += coeff * x;
  }
  const long long sign = (((n + 1) % 2) ? (-1) : 1);
  res.left *= sign;
  res.right *= (-1);
  return res;
}
LLTuple year23_day09(char *buf,  long buf_len) {
  LLTuple day_res = {0};
  char **curr_pos = &buf;
  while (true) {
    char *const line = strsep(curr_pos, "\n");
    if (line == NULL)
      break;
    const LLTuple res = get_extrapolated_val(line);
    day_res.left += res.left;
    day_res.right += res.right;
  }
  return day_res;
}
AocDayRes solve_year23_day09(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, false);
  LLTuple res =
      parallelize(year23_day09, ll_tuple_add, input_buffer, filesize, 0);

  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
