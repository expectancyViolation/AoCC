#include "year22.h"
#include "day01.h"
#include "day02.h"

AocDayRes year22_solver(const AocDayTask task) {
  const char *file = task.input_file;
  switch (task.day) {
  case 1:
    return solve_year22_day01(file);
  case 2:
    return solve_year22_day02(file);
  default:
    printf("unsolved y:%d d:%d\n", task.year, task.day);
  }

  AocDayRes res = {0};
  return res;
}