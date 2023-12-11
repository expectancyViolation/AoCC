#include "aoc_types.h"

AOC_DAY_PART AOC_DAY_PART_BOTH_PARTS[2]={AOC_DAY_PART_part1,AOC_DAY_PART_part2};

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
struct AocPartRes aoc_part_res_from_llong(long long int val) {
  struct AocPartRes res = {.type = AOC_PART_RES_TYPE_llong, .res_ll = val};
  return res;
}
AocDayRes aoc_day_res_from_tuple(const LLTuple *tup) {
  const AocDayRes result = {.part1_res = aoc_part_res_from_llong(tup->left),
                            .part2_res = aoc_part_res_from_llong(tup->right)};
  return result;
}
