#ifndef AOCC_RESULT_DB_H
#define AOCC_RESULT_DB_H

#include "aoc_types.h"
#include "helpers.h"
#include <limits.h>

#define RESULT_STATUS_VERSION "v0_0_1"
#define RESULT_STATUS_DEFAULT_YEAR 2023
#define RESULT_STATUS_SUBMISSION_HISTORY_SIZE 20

typedef void *result_db_handle;

enum solution_type {
  solution_type_unknown = 0,
  solution_type_num_solution = 1,
  solution_type_string_solution = 2
};

void print_solution_type(enum solution_type sol);

struct submission{
  char string_solution[AOC_SOL_MAX_LEN];
};

struct result_status {
  int year;
  int day;
  enum AOC_DAY_PART part;
  bool solved;
  enum solution_type solution_type;
  long long num_solution;
  long long num_solution_lower_bound;
  long long num_solution_upper_bound;
  char string_solution[100];
  struct submission submissions[RESULT_STATUS_SUBMISSION_HISTORY_SIZE];
  // struct timespec next_guess_cooldown; TODO: use this for auto-retry?
};

void print_result_status(const struct result_status *status);

result_db_handle result_db_init_db(char *db_file);

void result_db_close(result_db_handle handle);

void result_db_initialize_result_status(struct result_status *status);

char *result_status_get_id(const struct result_status *status);

bool result_status_load_entry(result_db_handle handle, int year, int day,
                              enum AOC_DAY_PART part,
                              struct result_status **out);

bool result_status_store_entry(result_db_handle handle,
                               const struct result_status *status,
                               bool overwrite);

void result_db_test();

#endif // AOCC_RESULT_DB_H
