#include "ll_tuple.h"

struct ll_tuple ll_tuple_add(const struct ll_tuple res1,
                             const struct ll_tuple res2) {
  struct ll_tuple res = {res1.left + res2.left, res1.right + res2.right};
  return res;
}
void print_tuple(const struct ll_tuple tup) {
  printf("(%lld, %lld)", tup.left, tup.right);
}
void ll_tuple_copy(const struct ll_tuple *to, struct ll_tuple *from) {
  memcpy((void *)to, (void *)from, sizeof *to);
}
