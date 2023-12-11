#ifndef AOCC_AOC_TYPES_H
#define AOCC_AOC_TYPES_H

#include "ll_tuple.h"

#include <stdbool.h>

#define AOC_SOL_MAX_LEN 100

typedef enum AOC_DAY_PART {
  AOC_DAY_PART_undefined = 0,
  AOC_DAY_PART_part1 = 1,
  AOC_DAY_PART_part2 = 2
} AOC_DAY_PART;

// TODO: do we _really_ need this?
//extern AOC_DAY_PART AOC_DAY_PART_BOTH_PARTS[2];
//
//#define NUM_AOC_DAY_PART_BOTH_PARTS (2)


typedef struct AocPartStatus {
  bool part_solved;
  char part_solution[AOC_SOL_MAX_LEN + 1];
} AocPartStatus;

typedef struct AocDayStatus {
  AocPartStatus part1_status;
  AocPartStatus part2_status;
} AocDayStatus;

typedef struct AocSubmissionStatus {
  bool was_checked;
  bool correct;
  bool too_low;
  bool too_high;
  bool already_complete;
} AocSubmissionStatus;

typedef struct AocDayTask {
  int year;
  int day;
  char *input_file;
} AocDayTask;

enum AOC_PART_RES_TYPE { AOC_PART_RES_TYPE_llong, AOC_PART_RES_TYPE_string };

#define NUM_AOC_PART_RES_TYPE (2);

typedef struct AocPartRes {
  enum AOC_PART_RES_TYPE type;
  union {
    long long res_ll;                     // type=llong
    char res_string[AOC_SOL_MAX_LEN + 1]; // type=string
  };
} AocPartRes;

// these are each other's inverse
void parse_aoc_part_res(const char *string, AocPartRes *out);
void format_aoc_part_res(const AocPartRes *res, char *out);
AocPartRes aoc_part_res_from_llong(long long val);

typedef struct AocDayRes {
  AocPartRes part1_res;
  AocPartRes part2_res;
} AocDayRes;

AocDayRes aoc_day_res_from_tuple(const LLTuple* tup);

#endif // AOCC_AOC_TYPES_H
