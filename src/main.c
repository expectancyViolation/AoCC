#include <curl/curl.h>

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
#include "util/aoc_solution_manager.h"
#include "util/result_db.h"
#include "util/timer.h"

#define CURRENT_DAY 9

// TODO: use function pointers instead of massive switch?
struct aoc_day_res master_solver(const struct aoc_day_task task) {
  const char *file = task.input_file;
  switch (task.year) {
  case 2023:
    switch (task.day) {
    case 1:
      return solve_day01(file);
    case 2:
      return solve_day02(file);
    case 3:
      return solve_day03(file);
    case 4:
      return solve_day04(file);
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
    default:
      printf("unsolved y:%d d:%d\n", task.year, task.day);
      assert(false);
    }
    break;
  default:
    assert(false);
  }
}

__attribute__((unused)) void run_all_days() {
  struct aoc_day_task *tasks = NULL;
  for (int i = 1; i <= CURRENT_DAY; i++) {
    const struct aoc_day_task task = {2023, i, get_input_file_path(2023, i)};
    cvector_push_back(tasks, task);
  }
  size_t num_of_tasks = CURRENT_DAY;
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

int main() {
  curl_global_init(CURL_GLOBAL_ALL);
  result_db_handle db = result_db_init_db("/tmp/ress.db");
  aoc_manager_handle manager_handle = aoc_manager_init_manager(db);

  char *day10_filepath = get_input_file_path(2023, 10);
  fetch_day_input_cached(2023, 10, day10_filepath);
  const struct aoc_day_res res = solve_day10("/tmp/aoc/2023/day10_example.txt");
  // const struct aoc_day_res res=solve_day10(day10_filepath);
  print_aoc_day_result(&res);
  char resp[100] = {};
  sprintf(resp, "%lld", res.result.left);
  printf("resp:%s", resp);
  submit_answer(2023, 10, day_part_part1, resp);

  //  test_aoc_parse();
  //  for (int day = 15; day < 26; day++) {
  //    //    const struct aoc_day_status status = fetch_day_status(2021, day);
  //    //    print_aoc_day_status(&status);
  //    aoc_manager_pull_day_status(manager_handle, 2016, day);
  //  }
  // submit_answer(2016,23,day_part_part1,"1234");

  // submit_answer(2016,23,day_part_part1,"12342");

  /*for (int day = 15; day < 26; day++) {
    struct result_status* result_status;
    result_status_load_entry(db,2016,day,day_part_part1,&result_status);
    print_result_status(result_status);
    free(result_status);

    result_status_load_entry(db,2016,day,day_part_part2,&result_status);
    print_result_status(result_status);
    free(result_status);
  }*/

  //  result_db_test();
  // run_all_days();
  //  struct aoc_day_res res=solve_day09("/tmp/aoc/2023/day09_input.txt");
  //  print_aoc_day_result(&res);
  curl_global_cleanup();
  result_db_close(db);
}
