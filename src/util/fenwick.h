#ifndef AOCC_FENWICK_H
#define AOCC_FENWICK_H

#include <stdio.h>

long fenwick_get(const long *tree, int i);

void fenwick_add_zero_range(long *tree, int tree_size, int i, long val);

void fenwick_add_range(long *tree, int tree_size, int lower, int upper,
                       long val);

__attribute__((unused)) void test_fenwick();

#endif // AOCC_FENWICK_H
