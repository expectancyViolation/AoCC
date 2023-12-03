//
// Created by matze on 02/12/2023.
//

#include "day01.h"
#include "day02.h"
#include "parallelize.h"
#include "two_part_result.h"

void print_day_result(char *day, struct two_part_result *result) {
  printf("----------\n");
  printf("%s\n", day);
  printf("---\n");
  print_two_part_result(result);
  printf("----------\n\n");
}

int main() {

  struct two_part_result *day1_res = allocate_two_part_result();
  // day 1
  parallelize((void *(*)(char *))(day01),
              (void (*)(void *, void *))(add_consume_partial_result), day1_res,
              "/tmp/day01");
  print_day_result("day01", day1_res);

  struct two_part_result *day2_res = allocate_two_part_result();
  // day 2
  parallelize((void *(*)(char *))(day02),
              (void (*)(void *, void *))(add_consume_partial_result), day2_res,
              "/tmp/day02");
  print_day_result("day02", day2_res);
}