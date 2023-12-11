//
// Created by matze on 10/12/2023.
//

#ifndef AOCC_AOC_TYPES_H
#define AOCC_AOC_TYPES_H

#include "ll_tuple.h"

#include <stdbool.h>

#define AOC_SOL_MAX_LEN 100


typedef struct aoc_day_res (*aoc_day_solve)();

enum AOC_DAY_PART {
  day_part_undefined = 0,
  day_part_part1 = 1,
  day_part_part2 = 2
};

struct aoc_part_status {
  bool part_solved;
  char part_solution[AOC_SOL_MAX_LEN];
};

struct aoc_day_status {
  struct aoc_part_status part1_status;
  struct aoc_part_status part2_status;
};

struct aoc_submission_status {
  bool was_checked;
  bool correct;
  bool too_low;
  bool too_high;
  bool already_complete;
};

struct aoc_day_task {
  int year;
  int day;
  char *input_file;
};

struct aoc_day_res {
  struct ll_tuple result;
};

#endif // AOCC_AOC_TYPES_H
