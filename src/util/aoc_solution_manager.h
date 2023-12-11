//
// Created by matze on 09/12/2023.
//

#ifndef AOCC_AOC_SOLUTION_MANAGER_H
#define AOCC_AOC_SOLUTION_MANAGER_H

#include "result_db.h"
typedef void *aoc_manager_handle;

aoc_manager_handle aoc_manager_init_manager(result_db_handle db_handle);

void aoc_manager_close(aoc_manager_handle handle);

void aoc_manager_pull_day_status(aoc_manager_handle handle, int year, int day);

struct aoc_submission_status aoc_manager_sane_submit(aoc_manager_handle handle, int year, int day,
                             enum AOC_DAY_PART part, const char *guess);

void aoc_manager_get_day_status(aoc_manager_handle handle, int year, int day,
                                enum AOC_DAY_PART part,
                                struct result_status **status);

#endif // AOCC_AOC_SOLUTION_MANAGER_H
