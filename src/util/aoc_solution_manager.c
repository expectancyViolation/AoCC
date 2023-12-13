#include "aoc_solution_manager.h"
#include "aoc.h"
#include "aoc_solving.h"
#include <stdlib.h>

typedef struct _AocManagerData {
  result_db_handle db_handle;
} AocManagerData;

AocManagerData *aoc_manager_deref_handle(aoc_manager_handle handle) {
  return (AocManagerData *)handle;
}

aoc_manager_handle aoc_manager_init_manager(result_db_handle db_handle) {
  AocManagerData *handle_data = malloc(sizeof(*handle_data));
  handle_data->db_handle = db_handle;
  return (aoc_manager_handle)handle_data;
}
void aoc_manager_close(aoc_manager_handle handle) {
  AocManagerData *data = aoc_manager_deref_handle(handle);
  free(data);
}

void handle_pulled_part_status(result_db_handle db_handle, int year, int day,
                               enum AOC_DAY_PART day_part,
                               const AocPartStatus *part_status) {
  ResultStatus res_status;
  result_status_init(&res_status);
  res_status.year = year;
  res_status.day = day;
  res_status.part = day_part;
  ResultStatus *read_status = NULL;
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
  AocManagerData *handle_data = aoc_manager_deref_handle(handle);
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
                                enum AOC_DAY_PART part, ResultStatus **status) {
  AocManagerData *handle_data = aoc_manager_deref_handle(handle);
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

submission_sanity_flag_array
aoc_manager_sane_submit(aoc_manager_handle handle, int year, int day,
                        enum AOC_DAY_PART part, const AocPartRes guess,
                        AocSubmissionStatus *out_status) {
  AocManagerData *handle_data = aoc_manager_deref_handle(handle);

  ResultStatus *read_status = NULL;
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
  bool refetch = update_result_status(read_status, &guess, out_status);
  result_status_store_entry(handle_data->db_handle, read_status, true);
  if (refetch) {
    printf("detected inconsistencies. refetching...");
    aoc_manager_pull_day_status(handle, year, day);
  }
  return sanity_array;
}

bool aoc_manager_validate_solution(aoc_manager_handle handle, int year, int day,
                                   enum AOC_DAY_PART part,
                                   const AocPartRes guess) {
  // do not fetch if guess is invalid
  // TODO is this the right place for business logic?
  if (guess.type == AOC_PART_RES_TYPE_none)
    return false;

  // first try to validate via site
  ResultStatus *read_status = NULL;
  aoc_manager_get_day_status(handle, year, day, part, &read_status);
  // TODO: do we validate by submitting?
  return validate_solution(&guess, read_status);
}
