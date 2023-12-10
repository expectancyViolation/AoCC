#ifndef AOCC_PARALLELIZE_H
#define AOCC_PARALLELIZE_H

#define PARALLELIZE_RUN_ON_COPY
#define PARALLEL_LOOP_COUNT 24

#include "helpers.h"

#include <omp.h>
#include <stdbool.h>
#include <string.h>

struct ll_tuple parallelize(struct ll_tuple solve(char *buffer, long buf_len),
                 struct ll_tuple reduce_partial_result(struct ll_tuple tup1, struct ll_tuple tup2),
                 char *const input_buffer, long filesize,
                 int overlap);

#endif // AOCC_PARALLELIZE_H
