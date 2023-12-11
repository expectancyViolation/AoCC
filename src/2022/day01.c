#include "day01.h"
LLTuple year22_day01(char *buf, __attribute__((unused)) long buf_len) {
  LLTuple result = {};
  char **buf_pos = &buf;
  long long curr = 0;
  long long res[4] = {0, 0, 0, 0};
  while (*buf_pos != NULL) {
    char *line = strsep(buf_pos, "\n");
    if ((*line) == 0) {
      res[0] = curr;
      qsort(res, 4, sizeof(long long), (__compar_fn_t)compare_long);
      curr = 0;
    } else {
      curr += strtol(line, NULL, 10);
    }
  }
  result.left = res[3];
  result.right = res[1] + res[2] + res[3];
  return result;
}
AocDayRes solve_year22_day01(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  LLTuple res = year22_day01(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
