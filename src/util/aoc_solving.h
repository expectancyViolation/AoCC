
#ifndef AOCC_AOC_SOLVING_H
#define AOCC_AOC_SOLVING_H

#include "aoc_solution_manager.h"
#include "aoc_types.h"
#include "result_db.h"

void parse_solution_into_status(const AocPartRes *res, ResultStatus *status);

void merge_aoc_and_db_status(const AocPartStatus *part_status,
                             const ResultStatus *db_status,
                             ResultStatus *res_status);

submission_sanity_flag_array
check_submission_sanity(const ResultStatus *status, const AocPartRes guess);

bool guess_and_solution_types_match(enum AOC_PART_RES_TYPE guess_type,
                                           enum SOLUTION_TYPE solution_type);

bool update_result_status(
    ResultStatus *status, const AocPartRes *guess,
    const AocSubmissionStatus *submission_status);

bool validate_solution(const AocPartRes* guess,const ResultStatus* result_status);

#endif // AOCC_AOC_SOLVING_H
