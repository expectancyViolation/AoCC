//
// Created by matze on 03/12/2023.
//

#ifndef AOCC_PARALLELIZE_H
#define AOCC_PARALLELIZE_H

#define PARALLELIZE_ON_COPY
#define PARALLEL_LOOP_COUNT 50

#include "helpers.h"
#include <stdbool.h>
#include <string.h>

// TODO: better to pass a buffer?
void parallelize(void *solve(char *buffer),
                 void consume_partial_result(void *result, void *partial),
                 void *result, const char *filename) {
  char *file_content;
  const long filesize = read_file_to_memory(filename, &file_content, true);
  const int n_chunks = PARALLEL_LOOP_COUNT;
  const long chunk_size = filesize / n_chunks;
#pragma omp parallel for
  for (int i = 0; i < n_chunks; ++i) {
    const long begin_pos = chunk_size * i;
    const long end_pos = min(chunk_size * (i + 1), filesize - 1);
    char *begin_pointer = file_content + begin_pos;
    char *end_pointer = file_content + end_pos;
    align_pointers_on_separator(file_content, &begin_pointer, &end_pointer);
    *end_pointer = 0;
#ifdef PARALLELIZE_ON_COPY
    // operate on a copy to prevent "strsep" etc. to mess stuff up: TODO: how
    // does this affect performance
    const ptrdiff_t segment_size = end_pointer - begin_pointer;
    char *copy_buffer = (char *)malloc(segment_size);
    strncpy(copy_buffer, begin_pointer, segment_size);
    copy_buffer[segment_size - 1] = 0;
    void *partial_res = solve(copy_buffer);
    free(copy_buffer);
#else
    void *partial_res = solve(begin_pointer);
#endif
#pragma omp critical
    consume_partial_result(result, partial_res);
  }
#pragma omp single
  { free(file_content); }
}

#endif // AOCC_PARALLELIZE_H
