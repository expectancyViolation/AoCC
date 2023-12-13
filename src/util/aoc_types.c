#include "aoc_types.h"

void parse_aoc_part_res(const char *string, AocPartRes *out) {
  char *endpos = string;
  const long long parsed_ll = (strtol(string, &endpos, 10));
  if (endpos == string) {
    // failed to parse
    out->type = AOC_PART_RES_TYPE_string;
    strncpy(out->res_string, string, AOC_SOL_MAX_LEN);
  } else {
    out->type = AOC_PART_RES_TYPE_llong;
    out->res_ll = parsed_ll;
  }
}
void format_aoc_part_res(const AocPartRes *res, char *out) {
  switch (res->type) {
  case AOC_PART_RES_TYPE_llong:
    sprintf(out, "%lld", res->res_ll);
    break;
  case AOC_PART_RES_TYPE_string:
    strncpy(out, res->res_string, AOC_SOL_MAX_LEN + 1);
  }
}
AocPartRes aoc_part_res_from_llong(long long int val) {
  if (val == AOC_LLONG_NO_ANSWER) {
    return (AocPartRes){.type = AOC_PART_RES_TYPE_none};
  }
  return (AocPartRes){.type = AOC_PART_RES_TYPE_llong, .res_ll = val};
}
AocDayRes aoc_day_res_from_tuple(const LLTuple *tup) {
  const AocDayRes result = {.part1_res = aoc_part_res_from_llong(tup->left),
                            .part2_res = aoc_part_res_from_llong(tup->right)};
  return result;
}

void print_aoc_day_status(const AocDayStatus *status) {
  printf("day status:\n"
         "\tpart1:%20s (%d)\n"
         "\tpart2:%20s (%d)\n",
         status->part1_status.part_solution, status->part1_status.part_solved,
         status->part2_status.part_solution, status->part1_status.part_solved);
}

void print_aoc_submission_status(const AocSubmissionStatus *status) {
  printf("Submission status:\n"
         "\twas checked:%d\n"
         "\tcorrect:%d\n"
         "\ttoo_low:%d\n"
         "\ttoo_high:%d\n",
         status->was_checked, status->correct, status->too_low,
         status->too_high);
}

void print_aoc_day_task(const AocDayTask *task) {
  printf("task:\n\tyear:\t%30d\n\tday:\t%30d\n\tfile:\t%30s\n", task->year,
         task->day, task->input_file);
}

void print_aoc_part_res(const AocPartRes *res) {
  switch (res->type) {
  case AOC_PART_RES_TYPE_llong:
    printf("%30lld", res->res_ll);
    return;
  case AOC_PART_RES_TYPE_string:
    printf("%30s", res->res_string);
    return;
  }
}

void print_aoc_day_result(const AocDayRes *result) {
  printf("result:\n\tpart1:");
  print_aoc_part_res(&(result->part1_res));
  printf("\n\tpart2:");
  print_aoc_part_res(&(result->part2_res));
  printf("\n");
}

void print_day_benchmark(const AocBenchmarkDay *res) {
  printf("benchmark:\n");
  print_aoc_day_task(&res->task);
  print_aoc_day_result(&res->result);
  printf("took:%f\n\n", res->solve_duration);
}
