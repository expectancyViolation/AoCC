
#ifndef AOCC_RESULT_STATUS_H
#define AOCC_RESULT_STATUS_H

#include "aoc_types.h"

#define RESULT_STATUS_SUBMISSION_HISTORY_SIZE 20

// TODO: this should probably be a protobuffer
#define RESULT_STATUS_VERSION "v0_0_1"

enum SOLUTION_TYPE {
  SOLUTION_TYPE_unknown = 0,
  SOLUTION_TYPE_num = 1,
  SOLUTION_TYPE_string = 2
};

typedef struct _Submission {
  char string_solution[AOC_SOL_MAX_LEN + 1];
} Submission;

typedef struct _ResultStatus {
  int year;
  int day;
  enum AOC_DAY_PART part;
  bool solved;
  enum SOLUTION_TYPE solution_type;
  long long num_solution;
  long long num_solution_lower_bound;
  long long num_solution_upper_bound;
  char string_solution[AOC_SOL_MAX_LEN + 1];
  Submission submissions[RESULT_STATUS_SUBMISSION_HISTORY_SIZE];
  // struct timespec next_guess_cooldown; TODO: use this for auto-retry?
} ResultStatus;

void print_solution_type(enum SOLUTION_TYPE sol);

void print_result_status(const ResultStatus *status);


char *result_status_combine_id(int year, int day, enum AOC_DAY_PART part);
char *result_status_get_id(const ResultStatus *status);
void result_db_initialize_result_status(ResultStatus *status);

ResultStatus *decode_result_status(char *b64_string);
char *encode_result_status(const ResultStatus *status);

#endif // AOCC_RESULT_STATUS_H
