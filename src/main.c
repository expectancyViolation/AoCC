#include <curl/curl.h>
#include <unistd.h>

#include "2023/day01.h"
#include "2023/day02.h"
#include "2023/day03.h"
#include "2023/day04.h"
#include "2023/day05.h"
#include "2023/day06.h"
#include "2023/day07.h"
#include "2023/day08.h"
#include "2023/day09.h"
#include "2023/day10.h"
#include "2023/day11.h"
#include "util/aoc.h"
#include "util/aoc_solution_manager.h"
#include "util/result_db.h"
#include "util/timer.h"

#define CURRENT_DAY 10

// TODO: use function pointers instead of massive switch?
struct aoc_day_res master_solver(const struct aoc_day_task task) {
  const char *file = task.input_file;
  switch (task.year) {
  case 2023:
    switch (task.day) {
    case 1:
      return solve_year23_day01(file);
    case 2:
      return solve_year23_day02(file);
    case 3:
      return solve_year23_day03(file);
    case 4:
      return solve_year23_day04(file);
    case 5:
      return solve_day05(file);
    case 6:
      return solve_day06(file);
    case 7:
      return solve_day07(file);
    case 8:
      return solve_day08(file);
    case 9:
      return solve_day09(file);
    case 10:
      return solve_day10(file);
    default:
      printf("unsolved y:%d d:%d\n", task.year, task.day);
      assert(false);
    }
    break;
  default:
    assert(false);
  }
}

void generate_tasks(struct aoc_day_task **tasks) {
  for (int i = 1; i <= CURRENT_DAY; i++) {
    const struct aoc_day_task task = {2023, i, get_input_file_path(2023, i)};
    cvector_push_back(*tasks, task);
  }
}

__attribute__((unused)) void run_all_days() {
  struct aoc_day_task *tasks;
  generate_tasks(&tasks);
  size_t num_of_tasks = cvector_size(tasks);
  struct aoc_benchmark_day *results = malloc(num_of_tasks * sizeof(*results));
#pragma omp parallel for
  for (size_t i = 0; i < num_of_tasks; i++) {
    const struct aoc_day_task task = tasks[i];
    fetch_day_input_cached(task.year, task.day, task.input_file);
    results[i] = benchmark_day(master_solver, task);
  }
  for (size_t i = 0; i < num_of_tasks; i++) {
    print_day_benchmark(&results[i]);
  }
  free(results);
}

void test_submission(aoc_manager_handle manager_handle) {
  enum AOC_DAY_PART parts[] = {day_part_part1, day_part_part2};
  const int year = 2017;
  struct result_status *status = NULL;
  for (int day = 1; day < 5; day++) {
    for (int i = 0; i < 2; i++) {
      aoc_manager_get_day_status(manager_handle, year, day, parts[i], &status);
      print_result_status(status);
    }
  }
  //  struct aoc_benchmark_day bench = benchmark_day(master_solver, task);
  struct aoc_submission_status sub_status;
  aoc_manager_sane_submit(manager_handle, year, 1, day_part_part1, "1238786787",
                          &sub_status);
  print_aoc_submission_status(&sub_status);
}

int solve_current_day(aoc_manager_handle manager_handle) {
  int current_day = 11;
  char const *file = "/tmp/lul.txt";
  fetch_day_input_cached(2023, current_day, file);
  const struct aoc_day_res res = solve_day11(file);
  print_aoc_day_result(&res);

  char sol[50] = {};
  sprintf(sol, "%lld", res.result.left);

  struct aoc_submission_status submission_status = {};
  bool submit_ok =
      aoc_manager_sane_submit(manager_handle, 2023, current_day, day_part_part1,
                              sol, &submission_status);

  if (submit_ok == 0) {
    print_aoc_submission_status(&submission_status);
  }

  sprintf(sol, "%lld", res.result.left);

  submit_ok = aoc_manager_sane_submit(manager_handle, 2023, current_day,
                                      day_part_part1, sol, &submission_status);

  if (submit_ok == 0) {
    print_aoc_submission_status(&submission_status);
  }
}

int main() {
  curl_global_init(CURL_GLOBAL_ALL);
  result_db_handle db = result_db_init_db("/tmp/other_aoc/ress.db");
  aoc_manager_handle manager_handle = aoc_manager_init_manager(db);
  solve_current_day(manager_handle);

  // run_all_days();

  //  struct aoc_day_task *tasks=NULL;
  //  generate_tasks(&tasks);
  //  const size_t num_of_tasks = cvector_size(tasks);
  //  struct result_status * status=NULL;
  //  struct aoc_submission_status  submission_status={};
  //
  //  aoc_manager_pull_day_status(manager_handle,2023,1);
  //  for (int i = 0; i < num_of_tasks; i++) {
  //    const struct aoc_day_task task = tasks[i];
  //    struct aoc_day_res res = master_solver(tasks[i]);
  //      char sol[50] = {};
  //      sprintf(sol, "%lld", res.result.left);
  //      aoc_manager_get_day_status(manager_handle,task.year,task.day,day_part_part1,&status);
  //      print_result_status(status);
  //      submission_sanity_flag_array
  //      submit_ok=aoc_manager_sane_submit(manager_handle, task.year,
  //      task.day,
  //                              day_part_part1, sol,&submission_status);
  //      if(submit_ok==0) {
  //        print_aoc_submission_status(&submission_status);
  //        printf("submitted solution. sleeping...\n");
  //        usleep(65*1000*1000);
  //      }
  //  }

  //  print_day_benchmark(&bench);
  curl_global_cleanup();
  result_db_close(db);
}
