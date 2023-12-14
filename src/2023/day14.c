#include "day14.h"
#include "../../res/hashmap.c/hashmap.h"
#include "../util/parallelize.h"

long long calc_load(char *col_vec, size_t len) {
  long long res = 0;
  for (size_t i = 0; i < len; i++) {
    char *vec = col_vec + (len * i);
    for (size_t j = 0; j < len; j++) {
      if (vec[j] == 'O') {
        res += len - j;
      }
    }
  }
  return res;
}

void simulate(char *col_vec, size_t row_len) {
  static int call_count = 0;
  call_count += 1;
  printf("call:%d\n", call_count);
  for (size_t i = 0; i < row_len; i++) {
    char *vec = col_vec + (i * row_len);

    int *free_pos = NULL;
    size_t lowest_free_pos = 0;
    for (size_t j = 0; j < row_len; j++) {
      switch (vec[j]) {
      case '#':
        cvector_clear(free_pos);
        lowest_free_pos = 0;
        break;
      case 'O': {
        if (cvector_size(free_pos) > lowest_free_pos) {
          const int tar_pos = free_pos[lowest_free_pos];
          // printf("moving %ld to %ld\n", j, tar_pos);
          lowest_free_pos += 1;
          vec[tar_pos] = vec[j];
          vec[j] = '.';
          cvector_push_back(free_pos, j);
        } else {
          //          printf("no free pos: %ld,(%ld/%ld)", j, lowest_free_pos,
          //                 cvector_size(free_pos));
        }
      } break;
      case '.':
        // printf("just push it\n");
        cvector_push_back(free_pos, j);
        break;
      default:
        printf("%ld", j);
        printf("(");
        putchar(vec[j]);
        printf(")<----\n");
        // assert(false);
      }
    }
  }
}

void transpose_square(char *m, size_t len) {
  for (size_t i = 0; i < len; i++) {
    for (size_t j = i + 1; j < len; j++) {
      const char tmp = m[i * len + j];
      m[i * len + j] = m[j * len + i];
      m[j * len + i] = tmp;
    }
  }
}

void flip_square(char *m, size_t len) {
  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0; j < len / 2; j++) {
      const char tmp = m[i * len + j];
      m[i * len + j] = m[(i + 1) * len - j - 1];
      m[(i + 1) * len - j - 1] = tmp;
    }
  }
}

void rot90_square(char *m, size_t len) {
  transpose_square(m, len);
  flip_square(m, len);
}

// TODO: in place? (or at least 2D (cont) array) transpose + flip
char **rot90(char **col_vec, size_t row_len) {
  assert(cvector_size(col_vec[0]) == row_len);
  char **rot_col_vec = malloc(row_len * sizeof(*rot_col_vec));
  memset(rot_col_vec, 0, row_len * sizeof(*col_vec));
  for (int i = row_len - 1; i >= 0; i--) {
    char *old_col = col_vec[i];
    for (size_t j = 0; j < row_len; j++) {
      cvector_push_back(rot_col_vec[j], old_col[j]);
    }
  }
  for (int i = 0; i < row_len; i++) {
    cvector_free(col_vec[i]);
  }
  free(col_vec);
  return rot_col_vec;
}

void print_col_vec(char *col_vec, size_t row_len) {
  printf("---\n");
  for (size_t i = 0; i < row_len; i++) {
    char *vec = col_vec + i * row_len;
    for (size_t j = 0; j < row_len; j++) {
      putchar(vec[j]);
    }
    printf("\n");
  }
}

typedef struct {
  u_int64_t hash;
  int cycle_count;
} CycleRes;

int cycle_res_compare(const void *a, const void *b, void *udata) {
  const CycleRes *sa = a;
  const CycleRes *sb = b;
  return CMP(sa->hash, sb->hash);
}
u_int64_t cycle_res_hash(const void *item, u_int64_t seed0, u_int64_t seed1) {
  const CycleRes *res = item;
  return res->hash;
}

char *step_cycle(char *col_vec, int row_len) {

  // printf("START STEP\n");
  for (int i = 0; i < 4; i++) {
    //    col_vec = rot90(col_vec, row_len);
    // we pull left (not up) so rot first then simulate

    simulate(col_vec, row_len);
    rot90_square(col_vec, row_len);
    // rot90_square(col_vec, row_len);
    // rot90_square(col_vec, row_len);
    // print_col_vec(col_vec, row_len);
    // printf("%lld\n", calc_load(col_vec,row_len));
  }

  // printf("DONE STEP!!!!!!!!!!!!!!!!");
  return col_vec;
}

LLTuple year23_day14(char *buf, long buf_len) {
  LLTuple res = {0, 0};
  size_t row_len = strchr(buf, '\n') - buf;
  // inefficient: use 2D array (fixed size) instead
  char *col_vec = malloc(sizeof(char) * row_len * row_len);
  int j = 0;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    for (size_t i = 0; i < row_len; i++) {
      col_vec[j * row_len + i] = line[i];
    }
    j++;
  }
  rot90_square(col_vec, row_len);
  rot90_square(col_vec, row_len);
  rot90_square(col_vec, row_len);
  // printf("%lld\n", calc_load(col_vec,row_len));
  struct hashmap *map = hashmap_new(sizeof(CycleRes), 0, 0, 0, cycle_res_hash,
                                    cycle_res_compare, NULL, NULL);
  int cycles_to_go = 1000000000;
  int cc = 0;
  while (cycles_to_go > 0) {
    // assert(cc < 3);
    col_vec = step_cycle(col_vec, row_len);
    cc++;
    uint64_t hash_val =
        hashmap_sip(col_vec, (sizeof(char)) * row_len * row_len, 123, 345667);
    CycleRes cycle_res = {.cycle_count = cc, .hash = hash_val};

    const CycleRes *old = hashmap_get(map, &(CycleRes){.hash = hash_val});
    if (old != NULL) {
      cycles_to_go = cycles_to_go % (cc - old->cycle_count);
      printf("\t\t\ttogo %d %d\n", cycles_to_go, (cc - old->cycle_count));
    }
    hashmap_set(map, &cycle_res);
    cycles_to_go--;
  }
  res.right = calc_load(col_vec, row_len);
  free(col_vec);
  return res;
}

AocDayRes solve_year23_day14(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  // printf("%s\n",input_buffer);

  // need new align for parallel
  //  LLTuple res =
  //      parallelize(year23_day12, ll_tuple_add, input_buffer, filesize, 0);

  LLTuple res = year23_day14(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
