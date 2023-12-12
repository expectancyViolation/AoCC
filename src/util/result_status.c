#include "result_status.h"
#include "../../res/b64.c/b64.h"
#include <limits.h>
#include <stdio.h>

#include "../../res/asprintf.c/asprintf.h"

void print_result_status(const ResultStatus *status) {
  assert(status != NULL);
  printf("status:\n"
         "\tyear:        %10d\n"
         "\tday:         %10d\n"
         "\tpart:        %10d\n"
         "\tsolved:      %10d\n",
         status->year, status->day, status->part, status->solved);
  printf("solution type: ");
  print_solution_type(status->solution_type);
  printf("\n");
  if (status->solved) {
    switch (status->solution_type) {
    case SOLUTION_TYPE_num:
      printf("\tsolution:    %10lld\n", status->num_solution);
      break;
    case SOLUTION_TYPE_string:
      printf("\tsolution:    %10s\n", status->string_solution);
      break;
    default:
      assert(false);
    }
  } else {
    printf("\tno solution so far :(\n");
  }
  if (status->solution_type == SOLUTION_TYPE_num) {
    printf("bounds\n"
           "\tlower_bound: %10lld\n"
           "\tupper_bound: %10lld\n",
           status->num_solution_lower_bound, status->num_solution_upper_bound);
  }
  printf("previous submissions:\n");
  // for (int i = 0; i < RESULT_STATUS_SUBMISSION_HISTORY_SIZE; i++) {
  for (int i = 0; i < 5; i++) {
    printf("%03d:%s\n", i, status->submissions[i].string_solution);
  }
  printf("....\n");
};

void print_solution_type(enum SOLUTION_TYPE sol) {
  switch (sol) {
  case SOLUTION_TYPE_num:
    printf("number");
    return;
  case SOLUTION_TYPE_string:
    printf("string");
    return;
  case SOLUTION_TYPE_unknown:
    printf("unknown");
    return;
  }
};

// TODO: platform dependent "serialization" :( aka just memcpy
char *encode_result_status(const ResultStatus *status) {
  char *enc = b64_encode((const unsigned char *)status, sizeof(*status));
  return enc;
}

ResultStatus *decode_result_status(char *b64_string) {
  ResultStatus *status =
      (ResultStatus *)b64_decode(b64_string, strlen(b64_string));
  return status;
}

char *result_status_combine_id(int year, int day, enum AOC_DAY_PART part) {
  char *res;
  asprintf(&res, "solve_%d_%02d_part_%d_%s", year, day, part,
           RESULT_STATUS_VERSION);
  return res;
}

char *result_status_get_id(const ResultStatus *status) {
  return result_status_combine_id(status->year, status->day, status->part);
};

void result_db_initialize_result_status(ResultStatus *status) {
  memset(status, 0, sizeof(*status));
  status->year = 2023;
  status->day = -1;
  status->part = AOC_DAY_PART_undefined;
  status->solved = false;
  status->num_solution_lower_bound = 1;
  status->num_solution_upper_bound = LLONG_MAX - 1;
  status->num_solution = -1;
  memset(status->string_solution, 0, AOC_SOL_MAX_LEN + 1);
}
