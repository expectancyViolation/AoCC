//
// Created by matze on 02/12/2023.
//

#include "day01.h"
#include "day02.h"
#include "parallelize.h"

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