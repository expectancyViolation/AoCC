#ifndef AOCC_23DAY08_H
#define AOCC_23DAY08_H

#include "../util/aoc.h"
#include "../util/aoc_types.h"
#include "../util/helpers.h"
#include "../util/ll_tuple.h"

#define _GNU_SOURCE
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_LEN 3

LLTuple day08(char *buf, __attribute__((unused)) long buf_len);

struct AocDayRes solve_day08(const char *input_file);

#endif // AOCC_23DAY08_H
