#include "day11.h"

struct aoc_day_res solve_day11(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, false);
  const struct ll_tuple res = day11(input_buffer, filesize);
  struct aoc_day_res day_res = {res};
  free(input_buffer);
  return day_res;
}

struct ll_tuple gen_position_counts(struct ll_tuple **pos,
                                    const struct ll_tuple *end) {
  assert(*pos != end);
  struct ll_tuple result = {(*pos)->left, 0};
  while ((*pos != end) && ((*pos)->left == result.left)) {
    (*pos) += 1;
    result.right += 1;
  }
  return result;
}

struct ll_tuple day11(char *buf, long buf_len) {
  struct ll_tuple result = {};
  const long line_length = strchr(buf, '\n') - buf + 1;
  char *curr_offset = buf;
  struct ll_tuple *positions = NULL;
  long long res = 0;
  while (true) {
    curr_offset = strchr(curr_offset, '#');
    if (curr_offset == NULL)
      break;
    const ptrdiff_t curr_pos_offset = curr_offset - buf;
    curr_offset += 1;
    const struct ll_tuple curr_pos = {curr_pos_offset / line_length,
                                      curr_pos_offset % line_length};
    for (int i = 0; i < cvector_size(positions); i++) {
      const struct ll_tuple old_pos = positions[i];
      const long long dist = llabs(curr_pos.left - old_pos.left) +
                             llabs(curr_pos.right - old_pos.right);
      res += dist;
    }
    cvector_push_back(positions, curr_pos);
  }

  long long p2_res=res;

  // two passes: swap tuples in between to scan both dimensions
  for (int i = 0; i < 2; i++) {
    qsort(positions, cvector_size(positions), sizeof(*positions),
          (__compar_fn_t)ll_tuple_compare);

    struct ll_tuple *curr_index = cvector_begin(positions);
    long long left = 0;
    long long right = cvector_size(positions);
    struct ll_tuple const *const end = cvector_end(positions);
    long long prev_pos = -1;
    while (curr_index != end) {
      const struct ll_tuple pos_cnt = gen_position_counts(&curr_index, end);
      const long long pos = pos_cnt.left;
      const long long dist = pos - prev_pos - 1;
      res += left * right * dist;
      p2_res+=left*right*dist*999999;

      left += pos_cnt.right;
      right -= pos_cnt.right;
      prev_pos = pos;
    }
    for (int j = 0; j < cvector_size(positions); j++) {
      const long long tmp = positions[j].left;
      positions[j].left = positions[j].right;
      positions[j].right = tmp;
    }
  }

  result.left = res;
  result.right=p2_res;

  cvector_free(positions);
  return result;
}
