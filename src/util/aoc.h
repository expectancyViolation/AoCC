#ifndef AOCC_AOC_H
#define AOCC_AOC_H

#include "aoc_parse.h"
#include "aoc_types.h"
#include "helpers.h"
#include "ll_tuple.h"
#include <stdbool.h>


AocBenchmarkDay benchmark_day(aoc_solver fun, AocDayTask task);

char *get_input_file_path(int year, int day);

void fetch_day_input(int year, int day, char const *outfile);
void fetch_day_input_cached(int year, int day, char const *filepath);
AocDayStatus fetch_day_status(int year, int day);
bool submit_answer(int year, int day, enum AOC_DAY_PART part,
                   const AocPartRes *guess, AocSubmissionStatus *out);

#endif // AOCC_AOC_H
