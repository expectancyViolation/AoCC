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
void aoc_manager_close(aoc_manager_handle handle) {
  struct aoc_manager_data *data = aoc_manager_deref_handle(handle);
  free(data);
}

void parse_solution_into_status(char *sol_string,
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
    res_status->solution_type = solution_type_no_solution;
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
