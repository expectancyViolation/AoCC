#ifndef AOCC_AOC_H
#define AOCC_AOC_H

#include "aoc_parse.h"
#include "aoc_types.h"
#include "helpers.h"
#include "ll_tuple.h"
#include <stdbool.h>

struct aoc_benchmark_day {
  struct aoc_day_task task;
  struct aoc_day_res result;
  double solve_duration;
};

typedef struct aoc_day_res (*aoc_solver)(const struct aoc_day_task task);

void print_aoc_day_status(const struct aoc_day_status *status);
void print_aoc_submission_status(const struct aoc_submission_status *status);
void print_aoc_day_task(const struct aoc_day_task *task);
void print_aoc_day_result(const struct aoc_day_res *result);
void print_day_benchmark(const struct aoc_benchmark_day *res);

struct aoc_benchmark_day benchmark_day(aoc_solver fun,
                                       struct aoc_day_task task);

char *get_input_file_path(int year, int day);

void fetch_day_input(int year, int day, char *outfile);
void fetch_day_input_cached(int year, int day, char *filepath);
struct aoc_day_status fetch_day_status(int year, int day);
bool submit_answer(int year, int day, enum AOC_DAY_PART part, const char *answer,struct aoc_submission_status* out);

#endif // AOCC_AOC_H
