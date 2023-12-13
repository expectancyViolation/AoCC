#include "day13.h"
#include "../util/parallelize.h"

void parse_block(char **curr_pos, int **outvec) {
  while ((*curr_pos) != NULL) {
    char *line = strsep(curr_pos, "\n");
    if (*line == 0) {
      break;
    }
    int lineval = 0;
    while (*line != 0) {
      lineval = 2 * lineval + (*line == '#');
      line++;
    }
    cvector_push_back(*outvec, lineval);
  }
}

LLTuple find_reflection_point(int *block_vec) {
  LLTuple res={0,0};
  const size_t s = cvector_size(block_vec);
  for (int i = 0; i < s - 1; i++) {
    int reflection_damage = 0;
    int a = i;
    int b = i + 1;
    while ((a >= 0) && (b < s)) {
      reflection_damage += __builtin_popcount(block_vec[a] ^ block_vec[b]);
      a--;
      b++;
    }
    if(reflection_damage==0){
      res.left=i+1;
    }
    if(reflection_damage==1){
      res.right=i+1;
    }
  }
  return res;
}

LLTuple year23_day13(char *buf, long buf_len) {
  LLTuple res = {0, 0};
  while (buf != NULL) {
    int *block_row_vec = NULL;
    size_t col_count = strchr(buf, '\n') - buf;
    parse_block(&buf, &block_row_vec);
    int *block_col_vec = NULL;
    for (int bitselect = col_count - 1; bitselect >= 0; bitselect--) {
      const int bitmask = 1 << bitselect;
      int col_val = 0;
      const int s = cvector_size(block_row_vec);
      for (int i = 0; i < s; i++) {
        col_val = 2 * col_val + ((block_row_vec[i] & bitmask) != 0);
      }
      cvector_push_back(block_col_vec, col_val);
    }
    LLTuple row_refl = find_reflection_point(block_row_vec);
    LLTuple col_refl = find_reflection_point(block_col_vec);
    res.left += 100*row_refl.left;
    res.right += 100*row_refl.right;
    res.left += col_refl.left;
    res.right += col_refl.right;

  }
  return res;
}

AocDayRes solve_year23_day13(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, false);

  // need new align for parallel
  //  LLTuple res =
  //      parallelize(year23_day12, ll_tuple_add, input_buffer, filesize, 0);

  LLTuple res = year23_day13(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
