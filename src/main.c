#include <curl/curl.h>
#include <unistd.h>

#include "2022/day01.h"

#include "2022/year22.h"
#include "2023/day11.h"
#include "2023/year23.h"
#include "util/aoc.h"
#include "util/aoc_solution_manager.h"
#include "util/result_db.h"
#include "util/timer.h"

#define CURRENT_DAY 11

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
  AocDayRes res = {};
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
  printf("P1:%s\t P2:%s\n", CHECKMARK(part1_correct), CHECKMARK(part2_correct));
  return part1_correct + part2_correct;
}

void run_all_days(aoc_manager_handle manager, int year) {
  AocDayTask *tasks = NULL;
  generate_tasks(&tasks, year);
  size_t num_of_tasks = cvector_size(tasks);
  AocBenchmarkDay *results = malloc(num_of_tasks * sizeof(*results));
#pragma omp parallel for
  for (size_t i = 0; i < num_of_tasks; i++) {
    const AocDayTask task = tasks[i];
    fetch_day_input_cached(task.year, task.day, task.input_file);
    results[i] = benchmark_day(master_solver, task);
  }
  for (size_t i = 0; i < num_of_tasks; i++) {
    const AocBenchmarkDay result = results[i];
    const AocDayTask task = tasks[i];
    bool both_correct = validate_day_result(manager, &(result.result), &task);
//    print_aoc_day_result(&result.result);
//    if (both_correct)
      print_day_benchmark(&results[i]);
  }
  free(results);
}

void test_submission(aoc_manager_handle manager_handle) {
  enum AOC_DAY_PART parts[] = {AOC_DAY_PART_part1, AOC_DAY_PART_part2};
  const int year = 2017;
  struct result_status *status = NULL;
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
  AocSubmissionStatus submission_status = {};
  submission_sanity_flag_array submit_ok = aoc_manager_sane_submit(
      manager_handle, year, day, part, guess, &submission_status);
  if (submit_ok == 0) {
    print_aoc_submission_status(&submission_status);
  }
}

void solve_current_day(aoc_manager_handle manager_handle) {
  const int current_day = 11;
  const int current_year = 2023;
  char const *file = "/tmp/day11_bigboy.txt";
  fetch_day_input_cached(current_year, current_day, file);
  const AocDayRes res = solve_year23_day11(file);
  print_aoc_day_result(&res);

  // part 1
  // submit_helper(manager_handle, current_year, current_day,
  // AOC_DAY_PART_part1,
  //              res.part1_res);

  // part 2
  // submit_helper(manager_handle, current_year, current_day,
  // AOC_DAY_PART_part2,
  //              res.part2_res);
}

int main() {
  curl_global_init(CURL_GLOBAL_ALL);
  result_db_handle db = result_db_init_db("/tmp/other_aoc/ress_vnnn.db");
  aoc_manager_handle manager_handle = aoc_manager_init_manager(db);
//  solve_current_day(manager_handle);

//    AocDayTask task = {
//        .year = 2023, .day = 11, .input_file = "/tmp/day11_bigboy.txt"};
//     AocBenchmarkDay bench=benchmark_day(master_solver, task);
//     print_day_benchmark(&bench);
  run_all_days(manager_handle, 2023);

  curl_global_cleanup();
  result_db_close(db);
}
