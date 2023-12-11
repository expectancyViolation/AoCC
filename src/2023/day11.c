#include "day11.h"

struct aoc_day_res solve_day11(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, false);
  const struct ll_tuple res = day11(input_buffer, filesize);
  struct aoc_day_res day_res = {res};
  free(input_buffer);
  return day_res;
}
struct ll_tuple day11(char *buf, long buf_len) {
  struct ll_tuple result = {};
  printf("%s\n", buf);
  const long line_length = strchr(buf, '\n') - buf + 1;
  char *curr_offset = buf;
  struct ll_tuple *positions = NULL;
  long long res = 0;
  while (true) {
    curr_offset = strchr(curr_offset, '#');
    printf("%x\n",curr_offset);
    if (curr_offset == NULL)
      break;
    const ptrdiff_t curr_pos_offset = curr_offset - buf;
    curr_offset+=1;
    const struct ll_tuple curr_pos = {curr_pos_offset / line_length,
                                      curr_pos_offset % line_length};
    for (int i = 0; i < cvector_size(positions); i++) {
      const struct ll_tuple old_pos = positions[i];
      const long long dist = llabs(curr_pos.left - old_pos.left) +
                             llabs(curr_pos.right - old_pos.right);
      res+=dist;
    }
    cvector_push_back(positions,curr_pos);
  }
  result.left=res;
  printf("%lld",res);
  cvector_free(positions);
  return result;
}
