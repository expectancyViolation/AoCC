#include "day03.h"

#include "../util/aoc.h"
#include "../util/parallelize.h"

static bool is_dig(char c) { return (c >= '0') && (c <= '9'); }

static int parse_number(char **curr_number_pos, char **curr_number_end) {
  while ((!is_dig(**curr_number_pos)) && (**curr_number_pos != 0))
    ++*curr_number_pos;
  if (**curr_number_pos == 0)
    return 0;
  const int number = (int)strtol(*curr_number_pos, curr_number_end, 10);
  return number;
}

static bool is_symbol(char c) {
  return ((c != '.') && (c != 0) && (c != '\n') && !is_dig(c));
}

static bool is_adjacent_to_symbol(char *segment_begin, const char *segment_end,
                                  long line_length) {
  bool result = false;
  for (int i = -1; i < 2; i++) {
    const long offset = i * line_length;
    for (char *ptr = segment_begin - 1; ptr != segment_end + 1; ++ptr) {
      result |= is_symbol(*(ptr + offset));
    }
  }
  return result;
}

typedef struct _GearMatch {
  char *pos;
  long num;
} GearMatch;

static int compare_gear_match(GearMatch *m1, GearMatch *m2) {
  return ((m1->pos) > (m2->pos)) - ((m1->pos) < (m2->pos));
}

static void day03_mark_gears(char *segment_begin, const char *segment_end,
                             long line_length, long value, GearMatch **vec) {
  GearMatch match;
  match.num = value;
  for (int i = -1; i < 2; i++) {
    const long offset = i * line_length;
    for (char *ptr = segment_begin - 1; ptr != segment_end + 1; ++ptr) {
      char *const curr_pos = (ptr + offset);
      if (*curr_pos == '*') {
        match.pos = curr_pos;
        cvector_push_back(*vec, match);
      }
    }
  }
}
LLTuple year23_day03(char *buf, long buf_len) {
  LLTuple result = {0};
  const long line_length = get_first_line_length(buf);

  char *curr_number_pos = buf;
  char *part1_segment_begin = curr_number_pos + 2 * line_length;
  char *safe_segment_begin = curr_number_pos + 1 * line_length;
  char *curr_number_end;

  char *part1_segment_end = buf + buf_len - 2 * line_length;
  char *safe_segment_end = buf + buf_len - 1 * line_length;

  // vector of matches
  GearMatch *v = NULL;

  int curr_number;
  while (true) {
    curr_number = parse_number(&curr_number_pos, &curr_number_end);
    if (curr_number == 0)
      break;

    const bool is_safe = (safe_segment_begin <= curr_number_pos) &&
                         (curr_number_pos < safe_segment_end);
    if (is_safe) {
      const bool is_adjacent =
          is_adjacent_to_symbol(curr_number_pos, curr_number_end, line_length);

      day03_mark_gears(curr_number_pos, curr_number_end, line_length,
                       curr_number, &v);

      if ((part1_segment_begin <= curr_number_pos) &&
          (curr_number_pos < part1_segment_end)) {
        if (is_adjacent) {
          result.left += curr_number;
        }
      }
    }
    curr_number_pos = curr_number_end;
  }

  qsort(v, cvector_size(v), sizeof(GearMatch),
        (__compar_fn_t)compare_gear_match);

  char *prev_pos = NULL;
  int curr_count = 0;
  long curr_prod = 1;
  if (v) {
    GearMatch *it;
    for (it = cvector_begin(v); it != cvector_end(v); ++it) {
      char *const curr_pos = it->pos;
      if (curr_pos != prev_pos) {
        if (curr_count == 2) {
          if ((part1_segment_begin <= prev_pos) &&
              (prev_pos < part1_segment_end))
            result.right += curr_prod;
        }
        curr_count = 0;
        curr_prod = 1;
      }
      curr_count += 1;
      curr_prod *= it->num;
      prev_pos = curr_pos;
    }
    if (curr_count == 2) {
      if ((part1_segment_begin <= prev_pos) && (prev_pos < part1_segment_end))
        result.right += curr_prod;
    }
  }

  return result;
}
AocDayRes solve_year23_day03(const char *input_file) {
  char *input_buffer;
  char *padded_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const long padded_buffer_len =
      pad_input(input_buffer, &padded_buffer, filesize, '.');

  const LLTuple res = year23_day03(padded_buffer, padded_buffer_len);

  // parallel seems to be broken :/
  //  LLTuple res = parallelize(year23_day03, ll_tuple_add,
  //  padded_buffer,
  //                                    padded_buffer_len, 0);
  free(padded_buffer);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);

  free(input_buffer);
  return day_res;
}
