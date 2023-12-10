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

#define CURRENT_DAY 10

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

  //  run_all_days();
  struct aoc_day_task task={2023,10,"/tmp/aoc_bigboy/2023/day10_input.txt"};
  struct aoc_benchmark_day bench=benchmark_day(master_solver,task);
  print_day_benchmark(&bench);
  curl_global_cleanup();
  result_db_close(db);
}
