#include "day05.h"

#include "../util/parallelize.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

struct Day05Range {
  long long begin;
  long long end;
};

struct Day05Mapping {
  struct Day05Range r;
  long long offset;
};

static bool range_is_empty(const struct Day05Range *r) {
  return (r->begin) >= (r->end);
}

static int range_compare_lex(const struct Day05Range *r1,
                             const struct Day05Range *r2) {
  const long diff_begin = (r1->begin) - (r2->begin);
  const long diff_end = (r1->end) - (r2->end);
  // prevent compare function overflow
  return (diff_begin != 0) ? (diff_begin > 0) - (diff_begin < 0)
                           : (diff_end > 0) - (diff_end < 0);
}

static int mapping_compare_lex(const struct Day05Mapping *m1,
                               const struct Day05Mapping *m2) {
  return range_compare_lex(&m1->r, &m2->r);
}

static void range_intersect(const struct Day05Range *r1,
                            const struct Day05Range *r2,
                            struct Day05Range *result) {
  result->begin = max(r1->begin, r2->begin);
  result->end = min(r1->end, r2->end);
}

static void shift_range(struct Day05Range *r, long offset) {
  r->begin += offset;
  r->end += offset;
}

static void parse_seeds_p1(char *line, struct Day05Range **vec) {
  struct Day05Range parsed_range;
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
}

static void parse_seeds_p2(char *line, struct Day05Range **vec) {
  struct Day05Range parsed_range;
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

// merge contiguous intervals
static void merge_seed_arr(struct Day05Range **v, struct Day05Range **out) {
  qsort(*v, cvector_size(*v), sizeof(struct Day05Range),
        (__compar_fn_t)range_compare_lex);
  struct Day05Range temp_range;
  temp_range.begin = (*v)[0].begin;
  temp_range.end = (*v)[0].end;
  for (size_t i = 1; i < cvector_size(*v); i++) {
    const struct Day05Range *curr_range = &(*v)[i];
    if (curr_range->begin > temp_range.end) {
      // temp_range closes
      cvector_push_back(*out, temp_range);
      temp_range.begin = curr_range->begin;
      temp_range.end = curr_range->end;
    } else {
      // coalesce
      temp_range.end = max(temp_range.end, curr_range->end);
    }
  }
  cvector_push_back(*out, temp_range);
}

static long long solve_mappings(void parse(char *line, struct Day05Range **vec),
                                char *buf) {
  char *seed_line = strsep(&buf, "\n");
  struct Day05Range *source = NULL;
  parse(seed_line, &source);
  struct Day05Range *dest = NULL;
  struct Day05Range temp_range;
  struct Day05Mapping temp_mapping;
  struct Day05Mapping *mappings = NULL;
  struct Day05Mapping *filled_mappings = NULL;

  strsep(&buf, "\n"); // remove first newline
  while (true) {
    if (buf == NULL)
      break;
    strsep(&buf, "\n"); // discard map title
    char *end_pos;
    cvector_clear(mappings);
    while (true) {
      long destination_range_begin = strtol(buf, &end_pos, 10);
      if (end_pos == buf)
        break;
      buf = end_pos;
      long source_range_begin = strtol(buf, &buf, 10);
      long range_len = strtol(buf, &buf, 10);
      temp_mapping.r.begin = source_range_begin;
      temp_mapping.r.end = source_range_begin + range_len;
      temp_mapping.offset = destination_range_begin - source_range_begin;
      cvector_push_back(mappings, temp_mapping);
    }
    qsort(mappings, cvector_size(mappings), sizeof(struct Day05Mapping),
          (__compar_fn_t)mapping_compare_lex);

    const struct Day05Mapping *m_it;
    const struct Day05Range *r_it;
    temp_mapping.r.begin =
        LLONG_MIN >> 2; // avoid overflow issues when shifting
    temp_mapping.r.end = LLONG_MIN >> 2;
    temp_mapping.offset = 0;
    cvector_clear(filled_mappings);
    for (m_it = cvector_begin(mappings); m_it != cvector_end(mappings);
         ++m_it) {
      if (m_it->r.begin > temp_mapping.r.end) {
        temp_mapping.r.begin = temp_mapping.r.end;
        temp_mapping.r.end = m_it->r.begin;
        temp_mapping.offset = 0;
        cvector_push_back(filled_mappings, temp_mapping);
      }
      temp_mapping.r.begin = m_it->r.begin;
      temp_mapping.r.end = m_it->r.end;
      temp_mapping.offset = m_it->offset;
      cvector_push_back(filled_mappings, temp_mapping);
    }
    temp_mapping.r.begin = temp_mapping.r.end;
    temp_mapping.r.end = LLONG_MAX >> 2;
    temp_mapping.offset = 0;

    cvector_push_back(filled_mappings, temp_mapping);

    // sort to intersect ranges by linear scan
    qsort(filled_mappings, cvector_size(filled_mappings),
          sizeof(struct Day05Mapping), (__compar_fn_t)mapping_compare_lex);
    qsort(source, cvector_size(source), sizeof(struct Day05Range),
          (__compar_fn_t)range_compare_lex);

    cvector_clear(dest);
    m_it = cvector_begin(filled_mappings);
    r_it = cvector_begin(source);
    for (m_it = cvector_begin(filled_mappings);
         m_it != cvector_end(filled_mappings); ++m_it) {
      // skip impossible intersections
      while ((r_it != cvector_end(source)) && (r_it->end <= m_it->r.begin))
        ++r_it;
      // handle possible intersections
      while ((r_it != cvector_end(source)) && (r_it->begin < m_it->r.end)) {
        range_intersect(&(m_it->r), r_it, &temp_range);
        shift_range(&temp_range, m_it->offset);
        if (!range_is_empty(&temp_range)) {
          cvector_push_back(dest, temp_range);
        }
        ++r_it;
      }
      // step back since last interval might extend into next Day05Mapping
      --r_it;
    }
    cvector_clear(source);

    // we need to merge arrays since our current result might contain
    // overlapping ranges,
    //   that cannot be handled by linear scanning:
    //   e.g. an earlier interval might extend further than the end on a further
    //   interval
    merge_seed_arr(&dest, &source);

    strsep(&buf, "\n"); // skip newline after last number
    strsep(&buf, "\n"); // skip empty line
  }
  long long min_el = LLONG_MAX;
  for (size_t i = 0; i < cvector_size(source); i++) {
    if (source[i].end > source[i].begin)
      min_el = min(min_el, source[i].begin);
  }

  cvector_free(source);
  cvector_free(dest);
  cvector_free(mappings);
  cvector_free(filled_mappings);
  return min_el;
}

AocDayRes solve_day05(const char *input_file) {
  LLTuple res = {};
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const size_t input_size = filesize * sizeof(char);
  char *input_copy;

  // part 1
  input_copy = malloc(input_size);
  memcpy(input_copy, input_buffer, input_size);
  res.left = solve_mappings(parse_seeds_p1, input_copy);

  // part 2
  memcpy(input_copy, input_buffer, input_size);
  res.right = solve_mappings(parse_seeds_p2, input_copy);
  free(input_copy);
  free(input_buffer);

  AocDayRes day_res = {res};
  return day_res;
}
