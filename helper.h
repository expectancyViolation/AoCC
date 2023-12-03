#ifndef AOCC_HELPER_H
#define AOCC_HELPER_H

#include <malloc.h>

#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })

#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })

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

void add_consume_partial_result(struct two_part_result *result,
                                struct two_part_result *partial_result) {
  (result->part2_result) += (partial_result->part2_result);
  (result->part1_result) += (partial_result->part1_result);
  free(partial_result);
}

void print_two_part_result(struct two_part_result *result) {
  printf("result:\npart1:\t%lld\npart2:\t%lld\n", result->part1_result,
         result->part2_result);
}

void print_day_result(char* day,struct two_part_result *result){
    printf("----------\n");
    printf("%s\n",day);
    printf("---\n");
    print_two_part_result(result);
    printf("----------\n\n");
}

#endif // AOCC_HELPER_H
