
#ifndef AOCC_DAY09_H
#define AOCC_DAY09_H

#include "../util/aoc.h"
#include "../util/helpers.h"
#include "../util/ll_tuple.h"

#define _GNU_SOURCE
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ll_tuple get_extrapolated_val(char *line) {
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
  struct ll_tuple res = {0, 0};
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

struct ll_tuple day09(char *buf, __attribute__((unused)) long buf_len) {
  struct ll_tuple day_res = {0, 0};
  char **curr_pos = &buf;
  while (true) {
    char *const line = strsep(curr_pos, "\n");
    if (line == NULL)
      break;
    const struct ll_tuple res = get_extrapolated_val(line);
    day_res.left += res.left;
    day_res.right += res.right;
  }
  return day_res;
}

void solve_day09() {
  const int year = 2023;
  const int day = 9;
  struct ll_tuple day_res = {};
  char *input_buffer;
  const long filesize = get_day_input_cached(year, day, &input_buffer);

  parallelize(day09, ll_tuple_add, input_buffer, filesize, 0);
  print_day_result(day, day_res);

  // part 1
  // submit_answer(2023, 9, day_part_part1, day_res);

  // part 2
  // submit_answer(2023, 9, day_part_part2, day_res);

  free(input_buffer);
}

#endif // AOCC_DAY09_H
