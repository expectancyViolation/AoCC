#include "aoc_solution_manager.h"
#include "aoc.h"
#include <stdlib.h>

enum submission_sanity_flags {
  SANITY_ALREADY_SOLVED = 1 << 0,
  SANITY_TOO_LOW = 1 << 1,
  SANITY_TOO_HIGH = 1 << 2,
  SANITY_WRONG_FORMAT = 1 << 3,
  SANITY_GUESS_ALREADY_TRIED = 1 << 4
  // TODO
};

typedef int submission_sanity_flag_array;

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
void aoc_manager_close(aoc_manager_handle handle) {
  struct aoc_manager_data *data = aoc_manager_deref_handle(handle);
  free(data);
}

void parse_solution_into_status(const char *sol_string,
                                struct result_status *status) {
  char *endpos;
  const long num = (strtol(sol_string, &endpos, 10));
  if (endpos == sol_string) {
    // failed to parse => degrade to string solution
    strncpy(status->string_solution, sol_string, AOC_SOL_MAX_LEN);
    status->solution_type = solution_type_string_solution;
  } else {
    status->num_solution = num;
    status->num_solution_lower_bound = num;
    status->num_solution_upper_bound = num;
    status->solution_type = solution_type_num_solution;
  }
}

// TODO: this is weird: better to store guesses and status separately?
void merge_aoc_and_db_status(const struct aoc_part_status *part_status,
                             const struct result_status *db_status,
                             struct result_status *res_status) {
  res_status->solved = part_status->part_solved;
  if (part_status->part_solved) {
    parse_solution_into_status(part_status->part_solution, res_status);
  } else {
    res_status->solution_type = solution_type_unknown;
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
                               const struct aoc_part_status *part_status) {
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
  struct aoc_day_status day_status = fetch_day_status(year, day);
  print_aoc_day_status(&day_status);
  handle_pulled_part_status(handle_data->db_handle, year, day, day_part_part1,
                            &day_status.part1_status);
  if (day != 25) {
    handle_pulled_part_status(handle_data->db_handle, year, day, day_part_part2,
                              &day_status.part2_status);
  }
}

void aoc_manager_get_day_status(aoc_manager_handle handle, int year, int day,
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
  assert(could_load);
}

submission_sanity_flag_array
check_submission_sanity(const struct result_status *status,
                        const char *submission) {
  submission_sanity_flag_array sanity_flags = 0;
  if (status->solved)
    sanity_flags |= SANITY_ALREADY_SOLVED;
  if (status->solution_type == solution_type_num_solution) {
    const char *endptr = submission;
    long submission_num = strtol(submission, &endptr, 10);
    if (endptr == submission) {
      // could not parse as number
      sanity_flags |= SANITY_WRONG_FORMAT;
    } else {
      if (submission_num < status->num_solution_lower_bound)
        sanity_flags |= SANITY_TOO_LOW;
      if (submission_num > status->num_solution_upper_bound)
        sanity_flags |= SANITY_TOO_HIGH;
    }
  }
  for (int i = 0; i < RESULT_STATUS_SUBMISSION_HISTORY_SIZE; i++) {
    if (strcmp(submission, status->submissions[i].string_solution) == 0) {
      sanity_flags |= SANITY_GUESS_ALREADY_TRIED;
      break;
    }
  }
  return sanity_flags;
}

void aoc_manager_update_result_status(
    struct result_status *status, const char *guess,
    const struct aoc_submission_status *submission_status) {
  if (!submission_status->was_checked) {
    // no new info
    return;
  }
  if (submission_status->correct) {
    parse_solution_into_status(guess, status);
  }
  bool out_of_bounds =
      (submission_status->too_low) || (submission_status->too_high);
  if (out_of_bounds) {
    long submission_num =
        strtol(guess, NULL, 10); // TODO: this might still fail
    status->solution_type = solution_type_num_solution;
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
  strncpy(status->submissions[0].string_solution, guess, AOC_SOL_MAX_LEN);
}

struct aoc_submission_status aoc_manager_sane_submit(aoc_manager_handle handle,
                                                     int year, int day,
                                                     enum AOC_DAY_PART part,
                                                     const char *guess) {
  struct aoc_manager_data *handle_data = aoc_manager_deref_handle(handle);

  struct result_status *read_status = NULL;
  aoc_manager_get_day_status(handle, year, day, part, &read_status);
  submission_sanity_flag_array sanity_array =
      check_submission_sanity(read_status, guess);
  printf("sanity:%x\n", sanity_array);
  if (sanity_array != 0) {
    print_result_status(read_status);
    assert("guess not sane.aborting..." == NULL);
  }
  struct aoc_submission_status submission_status =
      submit_answer(year, day, part, guess);
  aoc_manager_update_result_status(read_status, guess, &submission_status);
  printf("concluded:\n");
  print_result_status(read_status);
  result_status_store_entry(handle_data->db_handle, read_status, true);
  return submission_status;
}
