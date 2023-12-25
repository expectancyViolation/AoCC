#ifdef CURL_AVAILABLE
#include <curl/curl.h>
#endif

#include "2022/day01.h"

#include "2022/day02.h"
#include "2022/year22.h"
#include "2023/day14.h"
#include "2023/day15.h"
#include "2023/day16.h"
#include "2023/day17.h"
#include "2023/day18.h"
#include "2023/day19.h"
#include "2023/day20.h"
#include "2023/day21.h"
#include "2023/day22.h"
#include "2023/day23.h"
#include "2023/day24.h"
#include "2023/day25.h"
#include "2023/year23.h"
#include "util/aoc.h"
#include "util/aoc_solution_manager.h"
#include "util/result_db.h"
#include "util/timer.h"

#define CURRENT_DAY 25

#define CHECKMARK(x) ((x) ? ("✅") : ("❌"))

// TODO: use function pointers instead of massive switch?
AocDayRes master_solver(const AocDayTask task) {
  switch (task.year) {
  case 2022:
    return year22_solver(task);
  case 2023:
    return year23_solver(task);
  default:
    assert(false);
  }
  AocDayRes res = {0};
  return res;
}

void generate_tasks(AocDayTask **tasks, int year) {
  for (int i = 1; i <= CURRENT_DAY; i++) {
    const AocDayTask task = {year, i, get_input_file_path(year, i)};
    cvector_push_back(*tasks, task);
  }
}

bool validate_day_result(aoc_manager_handle manager, const AocDayRes *result,
                         const AocDayTask *task) {
  bool part1_correct = aoc_manager_validate_solution(
      manager, task->year, task->day, AOC_DAY_PART_part1, result->part1_res);
  bool part2_correct = aoc_manager_validate_solution(
      manager, task->year, task->day, AOC_DAY_PART_part2, result->part2_res);
  bool part1_no_guess = (result->part1_res.type == AOC_PART_RES_TYPE_none);
  char *part1_symbol = part1_no_guess ? "❓" : CHECKMARK(part1_correct);
  bool part2_no_guess = (result->part2_res.type == AOC_PART_RES_TYPE_none);
  char *part2_symbol = part2_no_guess ? "❓" : CHECKMARK(part2_correct);
  printf("P1:%5s\tP2:%5s\n", part1_symbol, part2_symbol);
  return !(part1_no_guess && part2_no_guess);
}

void run_all_days(aoc_manager_handle manager, int year) {
  AocDayTask *tasks = NULL;
  generate_tasks(&tasks, year);
  size_t num_of_tasks = cvector_size(tasks);
  AocBenchmarkDay *results = malloc(num_of_tasks * sizeof(*results));
#ifndef WIN32
// #pragma omp parallel for
#endif
  for (size_t i = 0; i < num_of_tasks; i++) {
    const AocDayTask task = tasks[i];
    fetch_day_input_cached(task.year, task.day, task.input_file);
    results[i] = benchmark_day(master_solver, task);
  }
  for (size_t i = 0; i < num_of_tasks; i++) {
    const AocBenchmarkDay result = results[i];
    const AocDayTask task = tasks[i];
    printf("-------\n");
    printf("DAY %2d:\n", task.day);
    bool made_guess = validate_day_result(manager, &(result.result), &task);
    if (made_guess) {
      print_day_benchmark(&results[i]);
      printf("took:%f\n\n", results[i].solve_duration);
    }
  }
  free(results);
}

void test_submission(aoc_manager_handle manager_handle) {
  enum AOC_DAY_PART parts[] = {AOC_DAY_PART_part1, AOC_DAY_PART_part2};
  const int year = 2017;
  ResultStatus *status = NULL;
  for (int day = 1; day < 5; day++) {
    for (int i = 0; i < 2; i++) {
      aoc_manager_get_day_status(manager_handle, year, day, parts[i], &status);
      print_result_status(status);
    }
  }
  //  AocBenchmarkDay bench = benchmark_day(master_solver, task);
  AocSubmissionStatus sub_status;
  const AocPartRes part_res = {.type = AOC_PART_RES_TYPE_string,
                               .res_string = "1234556789"};
  aoc_manager_sane_submit(manager_handle, year, 1, AOC_DAY_PART_part1, part_res,
                          &sub_status);
  print_aoc_submission_status(&sub_status);
}

void submit_helper(aoc_manager_handle manager_handle, int year, int day,
                   enum AOC_DAY_PART part, const AocPartRes guess) {
  AocSubmissionStatus submission_status = {0};
  submission_sanity_flag_array submit_ok = aoc_manager_sane_submit(
      manager_handle, year, day, part, guess, &submission_status);
  if (submit_ok == 0) {
    print_aoc_submission_status(&submission_status);
  }
}

void solve_current_day(aoc_manager_handle manager_handle) {
  const int current_day = 25;
  const int current_year = 2023;
  char const *file = "/tmp/d25_inp.txt";

  fetch_day_input_cached(current_year, current_day, file);
  const AocDayRes res = solve_year23_day25(file);
  print_aoc_day_result(&res);

  //  // part 1
  //  submit_helper(manager_handle, current_year, current_day,
  //  AOC_DAY_PART_part1,
  //                res.part1_res);

  // part 2
  //  submit_helper(manager_handle, current_year, current_day,
  //  AOC_DAY_PART_part2,
  //                res.part2_res);
}

int main() {
  // benchmark(result_db_test);
  // curl_global_init(CURL_GLOBAL_ALL);

  result_db_handle db = result_db_init_db("/tmp/other_aoc/res_vnnn__.db");
  aoc_manager_handle manager_handle = aoc_manager_init_manager(db);
  //  result_db_test();

  //  solve_current_day(manager_handle);

  //  char const *filepath = "/tmp/d23_input.txt";
  //  AocDayTask task_bb = {.year = 2023, .day = 23, .input_file = filepath};
  //  AocBenchmarkDay benchbb = benchmark_day(master_solver, task_bb);
  //  print_day_benchmark(&benchbb);
  run_all_days(manager_handle, 2023);

  // curl_global_cleanup();
  aoc_manager_close(manager_handle);
  result_db_close(db);
  return 0;
}
