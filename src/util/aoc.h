#ifndef AOCC_AOC_H
#define AOCC_AOC_H

#include "aoc_parse.h"
#include "aoc_types.h"
#include "helpers.h"
#include "ll_tuple.h"
#include <stdbool.h>

typedef struct AocBenchmarkDay {
  AocDayTask task;
  AocDayRes result;
  double solve_duration;
} AocBenchmarkDay;

typedef AocDayRes (*aoc_solver)(const AocDayTask task);

void print_aoc_day_status(const AocDayStatus *status);
void print_aoc_submission_status(const AocSubmissionStatus *status);
void print_aoc_day_task(const AocDayTask *task);
void print_aoc_part_res(const AocPartRes *res);
void print_aoc_day_result(const AocDayRes *result);
void print_day_benchmark(const AocBenchmarkDay *res);

AocBenchmarkDay benchmark_day(aoc_solver fun, AocDayTask task);

char *get_input_file_path(int year, int day);

void fetch_day_input(int year, int day, char const *outfile);
void fetch_day_input_cached(int year, int day, char const *filepath);
AocDayStatus fetch_day_status(int year, int day);
bool submit_answer(int year, int day, enum AOC_DAY_PART part,
                   const AocPartRes *guess, AocSubmissionStatus *out);

#endif // AOCC_AOC_H
