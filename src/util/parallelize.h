#ifndef AOCC_PARALLELIZE_H
#define AOCC_PARALLELIZE_H

#define PARALLELIZE_RUN_ON_COPY
#define PARALLEL_LOOP_COUNT 12

#include "ll_tuple.h"
#include "helpers.h"

#include <omp.h>
#include <stdbool.h>
#include <string.h>

LLTuple
parallelize(LLTuple solve(char *buffer, long buf_len),
                 LLTuple reduce_partial_result(LLTuple tup1, LLTuple tup2),
                 char *const input_buffer, long filesize,
                 int overlap);

#endif // AOCC_PARALLELIZE_H
