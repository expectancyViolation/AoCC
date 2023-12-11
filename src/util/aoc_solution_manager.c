#include "aoc_solution_manager.h"
#include "aoc.h"
#include <stdlib.h>

struct aoc_manager_data {
  result_db_handle db_handle;
};

struct aoc_manager_data *aoc_manager_deref_handle(aoc_manager_handle handle) {
  return (struct aoc_manager_data *)handle;
}

aoc_manager_handle aoc_manager_init_manager(result_db_handle db_handle) {
  struct aoc_manager_data *handle_data = malloc(sizeof(*handle_data));
  handle_data->db_handle = db_handle;
  return (aoc_manager_handle)handle_data;
}
__attribute__((unused)) void aoc_manager_close(aoc_manager_handle handle) {
  struct aoc_manager_data *data = aoc_manager_deref_handle(handle);
  free(data);
}

void parse_solution_into_status(const AocPartRes *res,
                                struct result_status *status) {
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
                             const struct result_status *db_status,
                             struct result_status *res_status) {
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

void handle_pulled_part_status(result_db_handle db_handle, int year, int day,
                               enum AOC_DAY_PART day_part,
                               const AocPartStatus *part_status) {
  struct result_status res_status;
  result_db_initialize_result_status(&res_status);
  res_status.year = year;
  res_status.day = day;
  res_status.part = day_part;
  struct result_status *read_status = NULL;
  // TODO: DANGER! no versioning of data
  bool could_load =
      result_status_load_entry(db_handle, year, day, day_part, &read_status);

  printf("could load: %d\n", could_load);
  if (read_status != NULL) {
    printf("got old status:\n");
    print_result_status(read_status);
  } else {
    printf("status is new!");
  }

  merge_aoc_and_db_status(part_status, read_status, &res_status);
  printf("updating status to:\n");
  print_result_status(&res_status);
  bool could_store = result_status_store_entry(db_handle, &res_status, true);
  printf("could store: %d", could_store);
}

void aoc_manager_pull_day_status(aoc_manager_handle handle, int year, int day) {
  if (day > 25)
    return;
  struct aoc_manager_data *handle_data = aoc_manager_deref_handle(handle);
  AocDayStatus day_status = fetch_day_status(year, day);
  print_aoc_day_status(&day_status);
  handle_pulled_part_status(handle_data->db_handle, year, day,
                            AOC_DAY_PART_part1, &day_status.part1_status);
  if (day != 25) {
    handle_pulled_part_status(handle_data->db_handle, year, day,
                              AOC_DAY_PART_part2, &day_status.part2_status);
  }
}

bool aoc_manager_get_day_status(aoc_manager_handle handle, int year, int day,
                                enum AOC_DAY_PART part,
                                struct result_status **status) {
  struct aoc_manager_data *handle_data = aoc_manager_deref_handle(handle);
  bool could_load =
      result_status_load_entry(handle_data->db_handle, year, day, part, status);
  if (!could_load) {
    printf("could not load day status. fetching...");
    aoc_manager_pull_day_status(handle, year, day);
  }

  could_load =
      result_status_load_entry(handle_data->db_handle, year, day, part, status);
  return could_load;
}

static bool guess_and_solution_types_match(enum AOC_PART_RES_TYPE guess_type,
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

static submission_sanity_flag_array
check_submission_sanity(const struct result_status *status,
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
  char guess_string[AOC_SOL_MAX_LEN + 1] = {};
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

bool aoc_manager_update_result_status(
    struct result_status *status, const AocPartRes *guess,
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

submission_sanity_flag_array
aoc_manager_sane_submit(aoc_manager_handle handle, int year, int day,
                        enum AOC_DAY_PART part, const AocPartRes guess,
                        AocSubmissionStatus *out_status) {
  struct aoc_manager_data *handle_data = aoc_manager_deref_handle(handle);

  struct result_status *read_status = NULL;
  aoc_manager_get_day_status(handle, year, day, part, &read_status);
  submission_sanity_flag_array sanity_array =
      check_submission_sanity(read_status, guess);
  printf("sanity:%x\n", sanity_array);
  if (sanity_array != 0) {
    print_result_status(read_status);
    // assert("guess not sane.aborting..." == NULL);
    return sanity_array;
  }
  submit_answer(year, day, part, &guess, out_status);
  bool refetch =
      aoc_manager_update_result_status(read_status, &guess, out_status);
  result_status_store_entry(handle_data->db_handle, read_status, true);
  if (refetch) {
    printf("detected inconsistencies. refetching...");
    aoc_manager_pull_day_status(handle, year, day);
  }
  return sanity_array;
}

// TODO split into "domain" (decider) and "service"
bool aoc_manager_validate_solution(aoc_manager_handle handle, int year, int day,
                                   enum AOC_DAY_PART part,
                                   const AocPartRes guess) {
  // first try to validate via site
  struct result_status *read_status = NULL;
  aoc_manager_get_day_status(handle, year, day, part, &read_status);
  if (!(read_status->solved)) {
    return false;
  }
  // TODO: do we trust cached info? (maybe optional "no_cache"?)
  if (!guess_and_solution_types_match(guess.type, read_status->solution_type)) {
//    printf("SOLUTION TYPE mismatch\n");
    return false;
  }

  switch (guess.type) {
  case AOC_PART_RES_TYPE_llong:
    return (read_status->num_solution) == guess.res_ll;
  case AOC_PART_RES_TYPE_string:
    return (read_status->string_solution) == guess.res_string;
  }
  return false;
}
