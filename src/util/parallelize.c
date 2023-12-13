#include "parallelize.h"

LLTuple parallelize(LLTuple (*solve)(char *, long),
                    LLTuple (*reduce_partial_result)(LLTuple, LLTuple),
                    char *const input_buffer, long filesize, int overlap) {
  struct _LLTuple result = {0, 0};
  const char *file_end = input_buffer + filesize - 1;
  const int n_chunks = PARALLEL_LOOP_COUNT;
  const long chunk_size = filesize / n_chunks;

#ifndef WIN32
#pragma omp parallel for
#endif
  for (int i = 0; i < n_chunks; ++i) {
    LLTuple partial_res = {0, 0};
    const long begin_pos = chunk_size * i;
    const long end_pos = min(chunk_size * (i + 1), filesize - 1);
    char *begin_pointer = input_buffer + begin_pos;
    char *end_pointer = input_buffer + end_pos;
    const bool is_first_segment = (i == 0);
    const int lower_extend = is_first_segment ? 1 : overlap;
    align_pointers_on_separator(input_buffer, file_end, lower_extend, overlap,
                                &begin_pointer, &end_pointer);
    if (!is_first_segment) {
      ++begin_pointer;
    }
#ifdef PARALLELIZE_RUN_ON_COPY
    // operate on a copy to prevent "strsep" etc. to mess stuff up:
    // TODO: how does this affect performance
    const ptrdiff_t segment_size = end_pointer - begin_pointer;
    assert(segment_size > 0);
    char *copy_buffer = (char *)malloc(segment_size + 1);
    strncpy(copy_buffer, begin_pointer, segment_size);
    copy_buffer[segment_size] = 0;
    // printf("%s\n\n",copy_buffer);
    partial_res = solve(copy_buffer, segment_size + 1);
    free(copy_buffer);
#else
    partial_res = solve(begin_pointer);
#endif
#ifndef WIN32
#pragma omp critical
#endif
    result = reduce_partial_result(result, partial_res);
  }
  return result;
}
