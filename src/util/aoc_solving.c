#include "aoc_solving.h"

#include "aoc.h"
#include "aoc_solution_manager.h"
#include <stdlib.h>

void parse_solution_into_status(const AocPartRes *res, ResultStatus *status) {
  status->solved = true;
  switch (res->type) {
  case AOC_PART_RES_TYPE_llong:
    status->num_solution = res->res_ll;
    status->num_solution_lower_bound = res->res_ll;
    status->num_solution_upper_bound = res->res_ll;
    status->solution_type = SOLUTION_TYPE_num;
    break;
  case AOC_PART_RES_TYPE_string:
    strncpy(status->string_solution, res->res_string, AOC_SOL_MAX_LEN + 1);
    status->solution_type = SOLUTION_TYPE_string;
  }
}
// TODO: this is weird: better to store guesses and status separately?
void merge_aoc_and_db_status(const AocPartStatus *part_status,
                             const ResultStatus *db_status,
                             ResultStatus *res_status) {
  res_status->solved = part_status->part_solved;
  if (part_status->part_solved) {
    AocPartRes part_res;
    parse_aoc_part_res(part_status->part_solution, &part_res);
    parse_solution_into_status(&part_res, res_status);
  } else {
    res_status->solution_type = SOLUTION_TYPE_unknown;
  }
  if (db_status != NULL) {
    if (!res_status->solved) {
      res_status->num_solution_lower_bound =
          db_status->num_solution_lower_bound;
      res_status->num_solution_upper_bound =
          db_status->num_solution_upper_bound;
    }
  }
}

bool guess_and_solution_types_match(enum AOC_PART_RES_TYPE guess_type,
                                    enum SOLUTION_TYPE solution_type) {
  switch (solution_type) {
  case SOLUTION_TYPE_unknown:
    return true;
  case SOLUTION_TYPE_num:
    return guess_type == AOC_PART_RES_TYPE_llong;
  case SOLUTION_TYPE_string:
    return guess_type == AOC_PART_RES_TYPE_string;
  }
  return false;
}

submission_sanity_flag_array check_submission_sanity(const ResultStatus *status,
                                                     const AocPartRes guess) {
  submission_sanity_flag_array sanity_flags = 0;
  if (status->solved)
    sanity_flags |= SANITY_ALREADY_SOLVED;
  if (!guess_and_solution_types_match(guess.type, status->solution_type)) {
    sanity_flags |= SANITY_WRONG_FORMAT;
  } else {
    if (guess.type == AOC_PART_RES_TYPE_llong) {
      if (guess.res_ll < status->num_solution_lower_bound)
        sanity_flags |= SANITY_TOO_LOW;
      if (guess.res_ll > status->num_solution_upper_bound)
        sanity_flags |= SANITY_TOO_HIGH;
    }
  }
  char guess_string[AOC_SOL_MAX_LEN + 1] = {0};
  switch (guess.type) {
  case AOC_PART_RES_TYPE_llong:
    sprintf(guess_string, "%lld", guess.res_ll);
    break;
  case AOC_PART_RES_TYPE_string:
    strncpy(guess_string, guess.res_string, AOC_SOL_MAX_LEN + 1);
  }
  for (int i = 0; i < RESULT_STATUS_SUBMISSION_HISTORY_SIZE; i++) {
    if (strcmp(guess_string, status->submissions[i].string_solution) == 0) {
      sanity_flags |= SANITY_GUESS_ALREADY_TRIED;
      break;
    }
  }
  return sanity_flags;
}
bool update_result_status(
    ResultStatus *status, const AocPartRes *guess,
    const AocSubmissionStatus *submission_status) {

  if (submission_status->already_complete && (!status->solved)) {
    // inconsistent state
    return true;
  }

  if (!submission_status->was_checked) {
    // no new info
    return false;
  }
  if (submission_status->correct) {
    parse_solution_into_status(guess, status);
  }
  bool out_of_bounds =
      (submission_status->too_low) || (submission_status->too_high);
  if (out_of_bounds) {
    long submission_num = guess->res_ll; // TODO: DANGER THIS MIGHT STILL FAIL
    status->solution_type = SOLUTION_TYPE_num;
    if (submission_status->too_low)
      status->num_solution_lower_bound = submission_num;
    if (submission_status->too_high)
      status->num_solution_upper_bound = submission_num;
  }
  // shift guesses
  for (int i = RESULT_STATUS_SUBMISSION_HISTORY_SIZE - 1; i >= 0; i--) {
    memcpy(status->submissions[i + 1].string_solution,
           status->submissions[i].string_solution, AOC_SOL_MAX_LEN);
  }
  format_aoc_part_res(guess, status->submissions[0].string_solution);
  return false;
}
bool validate_solution(const AocPartRes *guess,
                       const ResultStatus *result_status) {
  if (!(result_status->solved)) {
    return false;
  }
  // TODO: do we trust cached info? (maybe optional "no_cache"?)
  if (!guess_and_solution_types_match(guess->type, result_status->solution_type)) {
    //    printf("SOLUTION TYPE mismatch\n");
    return false;
  }

  switch (guess->type) {
    case AOC_PART_RES_TYPE_llong:
      return (result_status->num_solution) == guess->res_ll;
    case AOC_PART_RES_TYPE_string:
      return (result_status->string_solution) == guess->res_string;
  }
  return false;

}
