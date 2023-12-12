#include "year23.h"
#include "day01.h"
#include "day02.h"
#include "day03.h"
#include "day04.h"
#include "day05.h"
#include "day06.h"
#include "day07.h"
#include "day08.h"
#include "day09.h"
#include "day10.h"
#include "day11.h"
#include "day12.h"

AocDayRes year23_solver(const AocDayTask task) {
  const char *file = task.input_file;
  switch (task.day) {
  case 1:
    return solve_year23_day01(file);
  case 2:
    return solve_year23_day02(file);
  case 3:
    return solve_year23_day03(file);
  case 4:
    return solve_year23_day04(file);
  case 5:
    return solve_year23_day05(file);
  case 6:
    return solve_year23_day06(file);
  case 7:
    return solve_year23_day07(file);
  case 8:
    return solve_year23_day08(file);
  case 9:
    return solve_year23_day09(file);
  case 10:
    return solve_year23_day10(file);
  case 11:
    return solve_year23_day11(file);
  case 12:
    return solve_year23_day12(file);
  default:
    printf("unsolved y:%d d:%d\n", task.year, task.day);
    assert(false);
  }
  AocDayRes res = {0};
  return res;
}