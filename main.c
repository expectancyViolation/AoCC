//
// Created by matze on 02/12/2023.
//

#define PARALLELIZE_ON_COPY
#define PARALLEL_COUNT 24

#include "day01.h"
#include "day02.h"
#include <stddef.h>

void align_pointers_on_separator(const char *buf, char **head, char **tail) {
    if (*head != buf) {
        while ((**head != '\n') && (**head != 0))
            ++*head;
        ++*head;
    }
    while (**tail != '\n' && (**tail != 0))
        ++*tail;
}

// DANGER: free string after use
long read_file_to_memory(const char *filename, char **file_content_out,
                         bool append_newline) {
    FILE *f = fopen(filename, "rb");
    fseek(f, -1, SEEK_END);
    long fsize = ftell(f) + 1;
    char last_char = fgetc(f);
    fseek(f, 0, SEEK_SET);
    bool add_newline = append_newline && (last_char != '\n');
    *file_content_out = malloc(fsize + 1 + add_newline);
    fread(*file_content_out, fsize, 1, f);
    fclose(f);

    if (add_newline)
        (*file_content_out)[fsize - 1] = '\n';

    (*file_content_out)[fsize] = 0;
    return fsize;
}

// TODO: better to pass a buffer?
long long parallelize(long long solve(char *buffer),const char *filename) {
    char *file_content;
    const long filesize = read_file_to_memory(filename, &file_content, true);
    const int n_chunks = PARALLEL_COUNT;
    long long res = 0;
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
        // operate on a copy to prevent "strsep" etc. to mess stuff up: TODO: how does this affect performance
        const ptrdiff_t segment_size=end_pointer-begin_pointer;
        char* copy_buffer= (char*) malloc(segment_size);
        strncpy(copy_buffer,begin_pointer,segment_size);
        copy_buffer[segment_size-1]=0;
        const long long partial_res = solve(copy_buffer);
        free(copy_buffer);
#else
        const long long partial_res = solve(begin_pointer);
#endif
#pragma omp critical
        res += partial_res;
    }

#pragma omp single
    { free(file_content); }
    return res;

}

int main() {
    long long res;

    //day 1
    res=parallelize(day01,"/tmp/day01");
    printf("day01:\t%lld\n",res);

    //day 2
    res=parallelize(day02,"/tmp/day02");
    printf("day02:\t%lld\n",res);
}