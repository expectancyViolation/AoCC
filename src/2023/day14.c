#include "day14.h"
#include "../../res/hashmap.c/hashmap.h"
#include "../util/parallelize.h"



// parallelization is only worth it for large inputs
//#define D14_PARALLEL

long long calc_load(const char *mat, size_t len) {
  long long res = 0;

  for (size_t i = 0; i < len; i++) {
    const char *vec = mat + (len * i);
    for (size_t j = 0; j < len; j++) {
      if (vec[j] == 'O') {
        res += len - j;
      }
    }
  }
  return res;
}

void simulate(char *mat, size_t len) {
#ifdef D14_PARALLEL
#pragma omp parallel for
#endif
  for (size_t i = 0; i < len; i++) {
    char *const vec = mat + (i * len);

    size_t to_distribute = 0;
    for (int j = len - 1; j >= 0; j--) {
      switch (vec[j]) {
      case '#': {
        for (size_t k = 0; k < to_distribute; k++) {
          vec[j + k + 1] = 'O';
        }
        to_distribute = 0;
        break;
      }
      case 'O': {
        to_distribute++;
        vec[j] = '.';
        break;
      }
      }
    }
    for (size_t k = 0; k < to_distribute; k++) {
      vec[k] = 'O';
    }
  }
}

void transpose_square(char *m, size_t len) {
#ifdef D14_PARALLEL
#pragma omp parallel for
#endif
  for (size_t i = 0; i < len; i++) {
    for (size_t j = i + 1; j < len; j++) {
      const char tmp = m[i * len + j];
      m[i * len + j] = m[j * len + i];
      m[j * len + i] = tmp;
    }
  }
}

void flip_square(char *m, size_t len) {
#ifdef D14_PARALLEL
   #pragma omp parallel for
#endif
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

char *step_cycle(char *mat, size_t len) {
  for (int i = 0; i < 4; i++) {
    simulate(mat, len);
    rot90_square(mat, len);
  }
  return mat;
}

LLTuple year23_day14(char *buf, long buf_len) {
  LLTuple res = {0, 0};
  size_t len = strchr(buf, '\n') - buf;
  char *mat = malloc(sizeof(char) * len * len);
  int j = 0;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    for (size_t i = 0; i < len; i++) {
      mat[j * len + i] = line[i];
    }
    j++;
  }
  // rotate into place (simulate simulates falling to the "left")
  rot90_square(mat, len);
  rot90_square(mat, len);
  rot90_square(mat, len);

  // "quarter" step for part1 (simulate is idempotent)
  simulate(mat, len);
  res.left = calc_load(mat, len);

  struct hashmap *map = hashmap_new(sizeof(CycleRes), 0, 0, 0, cycle_res_hash,
                                    cycle_res_compare, NULL, NULL);
  int cycles_to_go = 1000000000;
  int cc = 0;
  while (cycles_to_go > 0) {
//    if(cc%100==0)
//      printf("cycle count: %d\n",cc);
    mat = step_cycle(mat, len);
    cc++;
    uint64_t hash_val = hashmap_sip(mat, (sizeof(char)) * len * len, 0, 0);

    CycleRes cycle_res = {.cycle_count = cc, .hash = hash_val};

    const CycleRes *old = hashmap_get(map, &(CycleRes){.hash = hash_val});
    if (old != NULL) {
      cycles_to_go = cycles_to_go % (cc - old->cycle_count);
    }
    hashmap_set(map, &cycle_res);
    cycles_to_go--;
  }
  res.right = calc_load(mat, len);
  free(mat);
  return res;
}

AocDayRes solve_year23_day14(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);

  LLTuple res = year23_day14(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
