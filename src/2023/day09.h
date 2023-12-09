
#ifndef AOCC_DAY09_H
#define AOCC_DAY09_H

#include "../util/aoc.h"
#include "../util/helpers.h"
#include "../util/two_part_result.h"

#define _GNU_SOURCE
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tuple {
  long long left;
  long long right;
};

struct tuple get_extrapolated_val(char *line) {
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
  struct tuple res = {0, 0};
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

struct two_part_result *day09(char *buf, long buf_len) {
  struct two_part_result *day_res = allocate_two_part_result();
  char **curr_pos = &buf;
  while (true) {
    char *const line = strsep(curr_pos, "\n");
    if (line == NULL)
      break;
    const struct tuple res = get_extrapolated_val(line);
    day_res->part1_result += res.left;
    day_res->part2_result += res.right;
  }
  return day_res;
}

void solve_day09() {
  const int year = 2023;
  const int day = 9;
  struct two_part_result *day_res = allocate_two_part_result();
  char *input_buffer;
  const long filesize = get_day_input_cached(year, day, &input_buffer);

  parallelize((void *(*)(char *, long))(day09),
              (void (*)(void *, void *))(add_consume_partial_result), day_res,
              input_buffer, filesize, 0);
  print_day_result(day, day_res);

  // part 1
  // submit_answer(2023, 9, day_part_part1, day_res);

  // part 2
  // submit_answer(2023, 9, day_part_part2, day_res);

  free_two_part_result(day_res);
  free(input_buffer);
}

#endif // AOCC_DAY09_H
