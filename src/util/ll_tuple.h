#ifndef AOCC_LL_TUPLE_H
#define AOCC_LL_TUPLE_H

#include <malloc.h>
#include <string.h>

struct ll_tuple {
  long long left;
  long long right;
};

void ll_tuple_copy(const struct ll_tuple* to,struct ll_tuple* from);

struct ll_tuple ll_tuple_add(const struct ll_tuple res1,
                           const struct ll_tuple res2);


void print_tuple(const struct ll_tuple tup);


#endif // AOCC_LL_TUPLE_H
