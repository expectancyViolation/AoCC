#ifndef AOCC_LL_TUPLE_H
#define AOCC_LL_TUPLE_H

#include "helpers.h"
#include <malloc.h>
#include <string.h>

typedef struct LLTuple {
  long long left;
  long long right;
}LLTuple;

void ll_tuple_copy(const LLTuple * to,LLTuple * from);

LLTuple ll_tuple_add(const LLTuple res1,
                           const LLTuple res2);

int ll_tuple_compare(const LLTuple * l1,const LLTuple * l2);;

int ll_tuple_compare_right_left(const LLTuple * l1,const LLTuple * l2);;

void print_tuple(const LLTuple tup);


#endif // AOCC_LL_TUPLE_H
