#include "day09.h"
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
struct ll_tuple day09(char *buf, long buf_len) {
  struct ll_tuple day_res = {};
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
struct aoc_day_res solve_day09(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, false);
  struct ll_tuple res =
      parallelize(day09, ll_tuple_add, input_buffer, filesize, 0);

  struct aoc_day_res day_res = {res};
  free(input_buffer);
  return day_res;
}
