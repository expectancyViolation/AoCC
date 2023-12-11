#ifndef AOCC_23DAY01_H
#define AOCC_23DAY01_H

#include "../util/aoc.h"
#include "../util/helpers.h"
#include "../util/ll_tuple.h"
#include "../util/parallelize.h"
#include "../util/aoc_types.h"

#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct ll_tuple year23_day01(char *buf, __attribute__((unused)) long buf_len);

struct aoc_day_res solve_year23_day01(const char *input_file);
#endif // AOCC_23DAY01_H
