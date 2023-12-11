#ifndef AOCC_AOC_TYPES_H
#define AOCC_AOC_TYPES_H

#include "ll_tuple.h"

#include <stdbool.h>

#define AOC_SOL_MAX_LEN 100

typedef struct AocDayRes (*aoc_day_solve)();

typedef enum AOC_DAY_PART {
  AOC_DAY_PART_undefined = 0,
  AOC_DAY_PART_part1 = 1,
  AOC_DAY_PART_part2 = 2
} AOC_DAY_PART;


typedef struct AocPartStatus {
  bool part_solved;
  char part_solution[AOC_SOL_MAX_LEN];
} AocPartStatus;

typedef struct AocDayStatus {
  struct AocPartStatus part1_status;
  struct AocPartStatus part2_status;
} AocDayStatus;

typedef struct AocSubmissionStatus {
  bool was_checked;
  bool correct;
  bool too_low;
  bool too_high;
  bool already_complete;
} AocSubmissionStatus;

typedef struct AocDayTask {
  int year;
  int day;
  char *input_file;
} AocDayTask;

typedef struct AocDayRes {
  LLTuple result;
} AocDayRes;

#endif // AOCC_AOC_TYPES_H
