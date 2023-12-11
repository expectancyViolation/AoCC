#ifndef AOCC_AOC_H
#define AOCC_AOC_H

#include "aoc_parse.h"
#include "aoc_types.h"
#include "helpers.h"
#include "ll_tuple.h"
#include <stdbool.h>

void print_aoc_day_status(const struct aoc_day_status *status);

char *get_input_file_path(int year, int day);

void fetch_day_input(int year, int day, char *outfile);
void fetch_day_input_cached(int year, int day, char *filepath);
struct aoc_day_status fetch_day_status(int year, int day);
struct aoc_submission_status
submit_answer(int year, int day, enum AOC_DAY_PART part,const char *answer);
#endif // AOCC_AOC_H
