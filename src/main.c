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
#include "util/timer.h"
#include "util/two_part_result.h"

__attribute__((unused)) void run_all_days() {

  void(*to_solve[]) = {parallel_solve_day01, parallel_solve_day02, solve_day03,
                       solve_day04,          solve_day05,          solve_day06,
                       solve_day07,          solve_day08,          solve_day09};
  size_t num_of_solves = sizeof(to_solve) / sizeof(to_solve[0]);
#pragma omp parallel for
  for (size_t i = 0; i < num_of_solves; i++) {
    benchmark(to_solve[i]);
  }
  printf("------------------\n\n");
}

int main() {
  curl_global_init(CURL_GLOBAL_ALL);
  benchmark(run_all_days);
  curl_global_cleanup();
}
