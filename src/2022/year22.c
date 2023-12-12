#include "year22.h"
#include "day01.h"

AocDayRes year22_solver(const AocDayTask task) {
  const char *file = task.input_file;
  switch (task.day) {
  case 1:
    return solve_year22_day01(file);
  case 2:
  default:
    printf("unsolved y:%d d:%d\n", task.year, task.day);
    assert(false);
  }

  AocDayRes res = {0};
  return res;
}