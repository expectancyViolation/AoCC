#ifndef AOCC_DAY06_H
#define AOCC_DAY06_H

#include "../util/aoc.c"
#include "../util/helpers.h"
#include "../util/ll_tuple.h"
#include "../util/aoc_types.h"


#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// to nudge floating point results in correct direction
#define EPS 0.00000000001

// from:
// https://stackoverflow.com/questions/1726302/remove-spaces-from-a-string-in-c
void remove_spaces(char *s) {
  char *d = s;
  do {
    while (*d == ' ') {
      ++d;
    }
  } while ((*s++ = *d++));
}

long long solve(char *buf) {
  char *line1 = strsep(&buf, "\n");
  char *line2 = buf;
  strsep(&line1, ":");
  strsep(&line2, ":");
  char *endpos;
  long long res = 1;
  while (true) {
    const double time = (double)(strtol(line1, &endpos, 10));
    if (endpos == line1)
      break;
    line1 = endpos;
    const double distance = (double)strtol(line2, &line2, 10);

    // solve quadratic equation x*(time-x)=distance;
    const double disc = time * time - 4 * distance;
    const long lower = ceil(((time - sqrt(disc)) / 2) + EPS);
    const long upper = floor((time + sqrt(disc)) / 2 - EPS);
    res *= (upper - lower + 1);
  }
  return res;
}

struct ll_tuple day06(char *buf, long buf_len) {
  struct ll_tuple day_res = {};
  const size_t buf_size = buf_len * sizeof(*buf);
  char *buf_copy = malloc(buf_size);
  memcpy(buf_copy, buf, buf_size);
  day_res.left = solve(buf_copy);
  remove_spaces(buf);
  day_res.right = solve(buf);
  free(buf_copy);
  return day_res;
}

struct aoc_day_res solve_day06(const char* input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const struct ll_tuple res = day06(input_buffer, filesize);
  free(input_buffer);

  struct aoc_day_res day_res={res};
  return day_res;
}

#endif // AOCC_DAY06_H
