#include "cvector.h"
#include "fenwick.h"
#include "helpers.h"
#include "parallelize.h"
#include "two_part_result.h"
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef AOCC_DAY05_H
#define AOCC_DAY05_H

#define DAY05_FILE "/tmp/day05"

struct range {
  long begin;
  long end;
};

bool range_is_empty(const struct range *r) { return (r->begin) >= (r->end); }

int range_compare_lex(const struct range *r1, const struct range *r2) {
  const long diff_begin = (r1->begin) - (r2->begin);
  const long diff_end = (r1->end) - (r2->end);
  // prevent compare function overflow
  return (diff_begin != 0) ? (diff_begin > 0) - (diff_begin < 0)
                           : (diff_end > 0) - (diff_end < 0);
}

void range_intersect(const struct range *r1, const struct range *r2,
                     struct range *result) {
  result->begin = max(r1->begin, r2->begin);
  result->end = min(r1->end, r2->end);
}

// this is only correct if r2 is not fully contained in r1
//      then we miss the right half...
//      but: worked on my input...
void range_diff(const struct range *r1, const struct range *r2,
                struct range *result) {
  if (r1->begin <= r2->begin) {
    result->begin = r1->begin;
    result->end = min(r1->end, r2->begin);
  } else {
    result->begin = max(r1->begin, r2->end);
    result->end = r1->end;
  }
}

void shift_range(struct range *r, long offset) {
  r->begin += offset;
  r->end += offset;
}

__attribute__((unused)) void parse_seeds_p1(char *line, struct range **vec) {
  struct range parsed_range;
  strsep(&line, ":");
  char *endpos;
  while (true) {
    const long curr_num = strtol(line, &endpos, 10);
    if (endpos == line)
      break;
    line = endpos;
    parsed_range.begin = curr_num;
    parsed_range.end = curr_num + 1;
    cvector_push_back(*vec, parsed_range);
  }
  printf("donso\n");
}

void parse_seeds_p2(char *line, struct range **vec) {
  struct range parsed_range;
  strsep(&line, ":");
  char *endpos;
  while (true) {
    parsed_range.begin = strtol(line, &endpos, 10);
    parsed_range.end = parsed_range.begin;
    if (endpos == line)
      break;
    line = endpos;
    parsed_range.end += strtol(line, &line, 10);
    cvector_push_back(*vec, parsed_range);
  }
}

void print_range(const struct range *r) {
  printf("range from %ld to %ld\n", r->begin, r->end);
}

__attribute__((unused)) void print_seed_arr(const struct range *arr) {
  for (size_t i = 0; i < cvector_size(arr); i++) {
    print_range(&arr[i]);
  }
  printf("\n");
}

void coalesce_seed_arr(struct range **v, struct range **out) {
  qsort(*v, cvector_size(*v), sizeof(struct range),
        (__compar_fn_t)range_compare_lex);
  struct range temp_range;
  temp_range.begin = (*v)[0].begin;
  temp_range.end = (*v)[0].end;
  for (size_t i = 1; i < cvector_size(*v); i++) {
    const struct range *curr_range = &(*v)[i];
    if (curr_range->begin > temp_range.end) {
      // temp_range closes
      cvector_push_back(*out, temp_range);
      temp_range.begin = curr_range->begin;
      temp_range.end = curr_range->end;
    } else {
      // coalesce
      temp_range.end = curr_range->end;
    }
  }
  cvector_push_back(*out, temp_range);
}

struct two_part_result *day05(char *buf,
                              __attribute__((unused)) long _buf_len) {
  struct two_part_result *day_res = allocate_two_part_result();
  char *line = strsep(&buf, "\n");
  struct range *source = NULL;
  struct range *dest = NULL;
  struct range parsed_source_range;
  struct range temp_range;

  parse_seeds_p2(line, &source);

  qsort(source, cvector_size(source), sizeof(struct range),
        (__compar_fn_t)range_compare_lex);
  print_seed_arr(source);
  strsep(&buf, "\n"); // remove first newline
  while (true) {
    if (buf == NULL)
      break;
    strsep(&buf, "\n"); // discard map title
    char *end_pos;
    while (true) {
      long destination_range_begin = strtol(buf, &end_pos, 10);
      if (end_pos == buf)
        break;
      buf = end_pos;
      long source_range_begin = strtol(buf, &buf, 10);
      long range_len = strtol(buf, &buf, 10);

      parsed_source_range.begin = source_range_begin;
      parsed_source_range.end = source_range_begin + range_len;
      const long offset = destination_range_begin - source_range_begin;
      for (size_t i = 0; i < cvector_size(source); i++) {
        struct range *curr_range = &source[i];
        range_intersect(curr_range, &parsed_source_range, &temp_range);
        if (!range_is_empty(&temp_range)) {
          shift_range(&temp_range, offset);
          cvector_push_back(dest, temp_range);
        }
        range_diff(curr_range, &parsed_source_range, &temp_range);
        curr_range->begin = temp_range.begin;
        curr_range->end = temp_range.end;
      }
    }
    for (size_t i = 0; i < cvector_size(source); i++) {
      const struct range *remaining_range = &source[i];
      if (!range_is_empty(remaining_range)) {
        cvector_push_back(dest, *remaining_range);
      }
    }
    cvector_clear(source);
    coalesce_seed_arr(&dest, &source);
    cvector_clear(dest);
    print_seed_arr(source);

    strsep(&buf, "\n"); // skip newline after last number
    strsep(&buf, "\n"); // skip empty line
  }
  long min_el = source[0].begin;
  for (size_t i = 0; i < cvector_size(source); i++) {
    min_el = min(min_el, source[i].begin);
  }
  day_res->part2_result = min_el;

  cvector_free(source);
  cvector_free(dest);
  return day_res;
}

void solve_day05() {
  struct two_part_result *day_res;
  char *input_buffer;
  // harder to parallelize b.c. of arbitrary range interactions!
  const long filesize = read_file_to_memory(DAY05_FILE, &input_buffer, false);
  day_res = day05(input_buffer, filesize);
  print_day_result("day05", day_res);
  free_two_part_result(day_res);
  free(input_buffer);
}

#endif // AOCC_DAY05_H
