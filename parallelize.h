//
// Created by matze on 03/12/2023.
//

#ifndef AOCC_PARALLELIZE_H
#define AOCC_PARALLELIZE_H

#define PARALLELIZE_ON_COPY
// TODO DANGER: buffer is split in equal size pieces,
//      if two pieces are contained within the same line
//      two threads will access the same segment of memory!
#define PARALLEL_LOOP_COUNT 24

#include "helpers.h"
#include <stdbool.h>
#include <string.h>
#include <omp.h>

// TODO: better to pass an input buffer?
void parallelize(void *solve(char *buffer, long buf_len),
                 void consume_partial_result(void *result, void *partial),
                 void *result, const char *input_buffer, long filesize,
                 int overlap) {
  const char *file_end = input_buffer + filesize - 1;
  const int n_chunks = PARALLEL_LOOP_COUNT;
  const long chunk_size = filesize / n_chunks;
#pragma omp parallel for
  for (int i = 0; i < n_chunks; ++i) {
    const long begin_pos = chunk_size * i;
    const long end_pos = min(chunk_size * (i + 1), filesize - 1);
    const char *begin_pointer = input_buffer + begin_pos;
    const char *end_pointer = input_buffer + end_pos;
    const bool is_first_segment = (i == 0);
    const int lower_extend = is_first_segment ? 1 : overlap;
    align_pointers_on_separator(input_buffer, file_end, lower_extend, overlap,
                                &begin_pointer, &end_pointer);
    if (!is_first_segment)
      ++begin_pointer;
#ifdef PARALLELIZE_ON_COPY
    // operate on a copy to prevent "strsep" etc. to mess stuff up:
    // TODO: how does this affect performance
    const ptrdiff_t segment_size = end_pointer - begin_pointer;
    char *copy_buffer = (char *)malloc(segment_size + 1);
    strncpy(copy_buffer, begin_pointer, segment_size);
    copy_buffer[segment_size] = 0;
    void *partial_res = solve(copy_buffer, segment_size + 1);
    free(copy_buffer);
//    int tid = omp_get_thread_num();
//    printf("Hello world from omp thread %d\n", tid);
#else
    void *partial_res = solve(begin_pointer);
#endif
#pragma omp critical
    consume_partial_result(result, partial_res);
  }
}

#endif // AOCC_PARALLELIZE_H
