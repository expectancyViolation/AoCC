//
// Created by matze on 02/12/2023.
//

#include "day01.h"
#include "day02.h"
#include "day03.h"
#include "parallelize.h"
#include "timer.h"
#include "two_part_result.h"

void print_day_result(char *day, struct two_part_result *result) {
  printf("----------\n");
  printf("%s\n", day);
  printf("---\n");
  print_two_part_result(result);
  printf("----------\n\n");
}

void parallel_solve_day01() {
  struct two_part_result *day_res = allocate_two_part_result();
  char *input_buffer;
  const long filesize = read_file_to_memory("/tmp/day01_bigboy", &input_buffer, true);
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
  const long filesize = read_file_to_memory("/tmp/day02_bigboy", &input_buffer, true);
  parallelize((void *(*)(char *, long))(day02),
              (void (*)(void *, void *))(add_consume_partial_result), day_res,
              input_buffer, filesize, 0);
  print_day_result("day02", day_res);
  free_two_part_result(day_res);
  free(input_buffer);
}

void solve_day03() {
  struct two_part_result *day_res = allocate_two_part_result();
  char *input_buffer;
  const long filesize =
      read_file_to_memory("/tmp/day03_bigboy", &input_buffer, true);
  char *padded_buffer;
  const long padded_buffer_len =
      day03_pad_input(input_buffer, &padded_buffer, filesize);
  free(input_buffer);
  parallelize((void *(*)(char *, long))(day03),
              (void (*)(void *, void *))(add_consume_partial_result), day_res,
              padded_buffer, padded_buffer_len, 2);
  print_day_result("day03", day_res);
  free_two_part_result(day_res);
  free(padded_buffer);
}

int main() {
  benchmark(parallel_solve_day01);
  benchmark(parallel_solve_day02);
  benchmark(solve_day03);
}