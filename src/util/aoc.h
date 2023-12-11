#ifndef AOCC_AOC_H
#define AOCC_AOC_H

#include "aoc_parse.h"
#include "aoc_types.h"
#include "helpers.h"
#include "ll_tuple.h"
#include <stdbool.h>

struct aoc_benchmark_day {
  struct AocDayTask task;
  struct AocDayRes result;
  double solve_duration;
};

typedef struct AocDayRes (*aoc_solver)(const struct AocDayTask task);

void print_aoc_day_status(const struct AocDayStatus *status);
void print_aoc_submission_status(const struct AocSubmissionStatus *status);
void print_aoc_day_task(const struct AocDayTask *task);
void print_aoc_day_result(const struct AocDayRes *result);
void print_day_benchmark(const struct aoc_benchmark_day *res);

struct aoc_benchmark_day benchmark_day(aoc_solver fun,
                                       struct AocDayTask task);

char *get_input_file_path(int year, int day);

void fetch_day_input(int year, int day, char *outfile);
void fetch_day_input_cached(int year, int day, char *filepath);
struct AocDayStatus fetch_day_status(int year, int day);
bool submit_answer(int year, int day, enum AOC_DAY_PART part, const char *answer,struct AocSubmissionStatus * out);

#endif // AOCC_AOC_H
