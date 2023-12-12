#include "ll_tuple.h"

LLTuple ll_tuple_add(const LLTuple res1,
                             const LLTuple res2) {
  LLTuple res = {res1.left + res2.left, res1.right + res2.right};
  return res;
}
 void print_tuple(const LLTuple tup) {
  printf("(%lld, %lld)", tup.left, tup.right);
}
 void ll_tuple_copy(const LLTuple *to, LLTuple *from) {
  memcpy((void *)to, (void *)from, sizeof *to);
}
int ll_tuple_compare(const LLTuple *l1, const LLTuple *l2) {
  const int left_cmp = CMP(l1->left, l2->left);
  if (left_cmp != 0)
    return left_cmp;
  return CMP(l1->right, l2->right);
}
 int ll_tuple_compare_right_left(const LLTuple *l1,
                                const LLTuple *l2) {
  const int right_cmp = CMP(l1->right, l2->right);
  if (right_cmp != 0)
    return right_cmp;
  return CMP(l1->left, l2->left);
}
