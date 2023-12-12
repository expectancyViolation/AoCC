#include "day12.h"
#include "../util/parallelize.h"

#define MAX_PATTERN_SIZE 10000

typedef struct _TempBuff {
  long long buf[MAX_PATTERN_SIZE][MAX_PATTERN_SIZE];
} TempBuff;

// dynamic programming:
//  temp_buf[i][j] =
//    possibilities to end up in at i in record and j in target_pattern
long long get_line_poss(char *record, char *target_pattern,
                        TempBuff *const buf) {
  const int record_len = strlen(record);
  const int pattern_len = strlen(target_pattern);
  (buf->buf)[0][0] = 1;
  for (int i = 0; i < record_len; i++) {
    const char record_symbol = record[i];
    const bool record_matches_ok =
        (record_symbol == '.') || (record_symbol == '?');
    const bool record_matches_nok =
        (record_symbol == '#') || (record_symbol == '?');
    const long long start_val = (buf->buf)[i][0] && record_matches_ok;
    (buf->buf)[i + 1][0] = start_val;
    int lower_bound = max(1, pattern_len - record_len + i);
    int upper_bound = min(pattern_len, i + 2);
    for (int j = lower_bound - 1; j < upper_bound; j++) {
      const char pattern_symbol = target_pattern[j];
      const bool can_skip = (pattern_symbol == '.') && record_matches_ok;
      const bool matches_nok = record_matches_nok && (pattern_symbol == '#');
      const bool matches_ok = record_matches_ok && (pattern_symbol == '.');
      const long long back_skip = (buf->buf)[i][j + 1];
      const long long back_match = (buf->buf)[i][j];
      const long long val =
          back_skip * can_skip + back_match * (matches_nok || matches_ok);
      (buf->buf)[i + 1][j + 1] = val;
    }
  }

  return (buf->buf)[record_len][pattern_len];
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

LLTuple year23_day12(char *buf, long buf_len) {
  LLTuple res = {0, 0};
  TempBuff *temp_buf = malloc(sizeof(*temp_buf));
  char **curr_pos = &buf;
  while (true) {
    char target_pattern[MAX_PATTERN_SIZE] = {0};
    char rec[MAX_PATTERN_SIZE] = {0};

    char *line = strsep(curr_pos, "\n");
    if ((line == NULL) || (*line == 0))
      break;

    char *record = strsep(&line, " ");
    fill_target_pattern(line, target_pattern);

    // extend pattern
    const size_t pattern_len = strlen(target_pattern);
    assert((pattern_len + 1) * 5 < MAX_PATTERN_SIZE);
    char extended_target_pattern[MAX_PATTERN_SIZE] = {0};
    char *tar_pos = extended_target_pattern;
    for (int i = 0; i < 5; i++) {
      memcpy(tar_pos, target_pattern, pattern_len);
      tar_pos += pattern_len;
    }

    // extend record
    const size_t rec_len = strlen(record);
    assert((rec_len + 1) * 5 < MAX_PATTERN_SIZE);
    char extended_record[MAX_PATTERN_SIZE] = {0};
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
    //    printf("\n%ld\n\n%ld\n",strlen(target_pattern),strlen(record));

    res.left += get_line_poss(record, target_pattern, temp_buf);
    res.right +=
        get_line_poss(extended_record, extended_target_pattern, temp_buf);
  }
  free(temp_buf);
  // print_tuple(res);
  // printf("\n");
  return res;
}

AocDayRes solve_year23_day12(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, false);
  LLTuple res =
      parallelize(year23_day12, ll_tuple_add, input_buffer, filesize, 0);

  //  LLTuple res = year23_day12(input_buffer,filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
