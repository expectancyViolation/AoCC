#ifndef AOCC_LL_TUPLE_H
#define AOCC_LL_TUPLE_H

#include <malloc.h>

struct ll_tuple {
  long long left;
  long long right;
};

struct ll_tuple ll_tuple_add(const struct ll_tuple res1,
                           const struct ll_tuple res2) {
  struct ll_tuple res = {res1.left + res2.left,
                                res1.right + res2.right};
  return res;
}

void print_two_part_result(const struct ll_tuple result) {
  printf("result:\npart1:\t%lld\npart2:\t%lld\n", result.left,
         result.right);
}

void print_day_result(int day, const struct ll_tuple result) {
  printf("--------------\n");
  printf("day%2d\n", day);
  print_two_part_result(result);
  printf("\n");
}

#endif // AOCC_LL_TUPLE_H
