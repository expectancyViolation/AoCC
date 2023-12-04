//
// Created by matze on 02/12/2023.
//

#include "day01.h"
#include "day02.h"
#include "day03.h"
#include "parallelize.h"
#include "timer.h"
#include "two_part_result.h"
#include "day04.h"
#include "fenwick.h"

void parallel_solve_day01() {
  struct two_part_result *day_res = allocate_two_part_result();
  char *input_buffer;
  const long filesize =
      read_file_to_memory("/tmp/day01_bigboy", &input_buffer, true);
  parallelize((void *(*)(char *, long))(day01),
              (void (*)(void *, void *))(add_consume_partial_result), day_res,
              input_buffer, filesize, 0);
  print_day_result("day01", day_res);
  free_two_part_result(day_res);
  free(input_buffer);
}

void parallel_solve_day02() {
  struct two_part_result *day_res = allocate_two_part_result();
  char *input_buffer;
  const long filesize =
      read_file_to_memory("/tmp/day02_bigboy", &input_buffer, true);
  parallelize((void *(*)(char *, long))(day02),
              (void (*)(void *, void *))(add_consume_partial_result), day_res,
              input_buffer, filesize, 0);
  print_day_result("day02", day_res);
  free_two_part_result(day_res);
  free(input_buffer);
}

int main() {
//  benchmark(parallel_solve_day01);
//  benchmark(parallel_solve_day02);
//  benchmark(solve_day03);
//test_fenwick();
    benchmark(solve_day04);
}