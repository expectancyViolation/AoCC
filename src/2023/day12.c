#include "day12.h"

#define MAX_PATTERN_SIZE 200

typedef struct _TempBuff {
  long long buf[MAX_PATTERN_SIZE][MAX_PATTERN_SIZE];
} TempBuff;

// dynamic programming:
//  temp_buf[i][j] =
//    possibilities to end up in at i in record and j in target_pattern
long long get_line_poss(char *record, char *target_pattern, TempBuff *buf) {
  const size_t record_len = strlen(record);
  const size_t pattern_len = strlen(target_pattern);
  buf->buf[0][0] = 1;
  for (size_t i = 1; i <= record_len; i++) {
    const char record_symbol = record[i - 1];
    const bool record_matches_ok =
        (record_symbol == '.') || (record_symbol == '?');
    const bool record_matches_nok =
        (record_symbol == '#') || (record_symbol == '?');
    const long long start_val = buf->buf[i - 1][0] && record_matches_ok;
    buf->buf[i][0] = start_val;
    for (size_t j = 1; j <= pattern_len; j++) {
      const char pattern_symbol = target_pattern[j - 1];
      const bool can_skip = (pattern_symbol == '.') && record_matches_ok;
      const bool matches_nok = record_matches_nok && (pattern_symbol == '#');
      const bool matches_ok = record_matches_ok && (pattern_symbol == '.');
      const long long val =
          buf->buf[i - 1][j] * can_skip +
          buf->buf[i - 1][j - 1] * (matches_nok || matches_ok);
      buf->buf[i][j] = val;
    }
  }

  return buf->buf[record_len][pattern_len];
}

// e.g. 3,2,1 => ###.##.#.
void fill_target_pattern(char *nums, char *out_target_pattern) {
  char *end_pos = nums;
  while (true) {
    long group_len = strtol(nums, &end_pos, 10);
    if (end_pos == nums)
      break;
    nums = end_pos + 1; // skip comma
    memset(out_target_pattern, '#', group_len);
    out_target_pattern += group_len;
    (*out_target_pattern) = '.';
    out_target_pattern++;
  }
  *out_target_pattern = 0;
}

LLTuple year23_day12(char *buf, __attribute__((unused)) long buf_len) {
  LLTuple res = {0, 0};
  TempBuff temp_buf = {};
  char **curr_pos = &buf;
  while (true) {
    char target_pattern[MAX_PATTERN_SIZE] = {};
    char rec[MAX_PATTERN_SIZE] = {};

    char *line = strsep(curr_pos, "\n");
    if (*line == 0)
      break;

    char *record = strsep(&line, " ");
    fill_target_pattern(line, target_pattern);

    // extend pattern
    const size_t pattern_len = strlen(target_pattern);
    assert((pattern_len + 1) * 5 < MAX_PATTERN_SIZE);
    char extended_target_pattern[MAX_PATTERN_SIZE] = {};
    char *tar_pos = extended_target_pattern;
    for (int i = 0; i < 5; i++) {
      memcpy(tar_pos, target_pattern, pattern_len);
      tar_pos += pattern_len;
    }

    // extend record
    const size_t rec_len = strlen(record);
    assert((rec_len + 1) * 5 < MAX_PATTERN_SIZE);
    char extended_record[MAX_PATTERN_SIZE] = {};
    char *ext_pos = extended_record;
    for (int i = 0; i < 5; i++) {
      memcpy(ext_pos, record, rec_len);
      ext_pos[rec_len] = '?';
      ext_pos += rec_len + 1;
    }

    // additional "end" state
    memcpy(rec, record, rec_len);
    rec[rec_len] = '.';
    record = rec;

    // end state
    extended_target_pattern[strlen(extended_target_pattern) - 1] = '.';

    res.left += get_line_poss(record, target_pattern, &temp_buf);
    res.right +=
        get_line_poss(extended_record, extended_target_pattern, &temp_buf);
  }
  return res;
}

AocDayRes solve_year23_day12(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, false);
  const LLTuple res = year23_day12(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
