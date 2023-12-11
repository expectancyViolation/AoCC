#ifndef AOCC_AOC_SOLUTION_MANAGER_H
#define AOCC_AOC_SOLUTION_MANAGER_H

#include "result_db.h"
typedef void *aoc_manager_handle;

enum submission_sanity_flags {
  SANITY_ALREADY_SOLVED = 1 << 0,
  SANITY_TOO_LOW = 1 << 1,
  SANITY_TOO_HIGH = 1 << 2,
  SANITY_WRONG_FORMAT = 1 << 3,
  SANITY_GUESS_ALREADY_TRIED = 1 << 4
  // TODO
};

typedef int submission_sanity_flag_array;

aoc_manager_handle aoc_manager_init_manager(result_db_handle db_handle);

__attribute__((unused)) void aoc_manager_close(aoc_manager_handle handle);

void aoc_manager_pull_day_status(aoc_manager_handle handle, int year, int day);

submission_sanity_flag_array
aoc_manager_sane_submit(aoc_manager_handle handle, int year, int day,
                        enum AOC_DAY_PART part, AocPartRes guess,
                        AocSubmissionStatus *out_status);


bool
aoc_manager_validate_solution(aoc_manager_handle handle,int year,int day,enum AOC_DAY_PART part,AocPartRes guess);

bool aoc_manager_get_day_status(aoc_manager_handle handle, int year, int day,
                                enum AOC_DAY_PART part,
                                struct result_status **status);

#endif // AOCC_AOC_SOLUTION_MANAGER_H
