#include "day01.h"
#include "day02.h"
#include "day03.h"
#include "day04.h"
#include "day05.h"
#include "parallelize.h"
#include "timer.h"
#include "two_part_result.h"
#include "day06.h"

#define DAY01_FILE "/tmp/day01"
#define DAY02_FILE "/tmp/day02"

void parallel_solve_day01() {
  struct two_part_result *day_res = allocate_two_part_result();
  char *input_buffer;
  const long filesize = read_file_to_memory(DAY01_FILE, &input_buffer, true);
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
  const long filesize = read_file_to_memory(DAY02_FILE, &input_buffer, true);
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
  //  benchmark(solve_day04);

  benchmark(solve_day05);
  //benchmark(solve_day06);
}