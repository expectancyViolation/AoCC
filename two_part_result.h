//
// Created by matze on 03/12/2023.
//

#ifndef AOCC_TWO_PART_RESULT_H
#define AOCC_TWO_PART_RESULT_H

#include <malloc.h>

struct two_part_result {
  long long part1_result;
  long long part2_result;
};

struct two_part_result *allocate_two_part_result() {
  struct two_part_result *result = malloc(sizeof *result);
  result->part1_result = 0;
  result->part2_result = 0;
  return result;
}

void free_two_part_result(struct two_part_result *result) { free(result); }

void add_consume_partial_result(struct two_part_result *result,
                                struct two_part_result *partial_result) {
  (result->part2_result) += (partial_result->part2_result);
  (result->part1_result) += (partial_result->part1_result);
  free_two_part_result(partial_result);
}

void print_two_part_result(struct two_part_result *result) {
  printf("result:\npart1:\t%lld\npart2:\t%lld\n", result->part1_result,
         result->part2_result);
}

void print_day_result(char *day, struct two_part_result *result) {
    printf("----------\n");
    printf("%s\n", day);
    printf("---\n");
    print_two_part_result(result);
    printf("----------\n\n");
}

#endif // AOCC_TWO_PART_RESULT_H
