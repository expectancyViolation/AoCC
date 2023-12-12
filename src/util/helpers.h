#ifndef AOCC_HELPERS_H
#define AOCC_HELPERS_H

#include "../../res/cvector.h"
#include <errno.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#ifndef WIN32
#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })

#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })
#endif

#ifdef WIN32
#define F_OK    0
#define __compar_fn_t int(*)(const void*,const void*)
// from https://stackoverflow.com/questions/8512958/is-there-a-windows-variant-of-strsep-function
char* strsep(char** stringp, const char* delim);
#endif

#define CMP(a, b) (((a) > (b)) - ((b) > (a)))


// DANGER: free string after use
long read_file_to_memory(const char *filename, char **file_content_out,
                         bool strip_newline);

void step_forward_to_separator(const char *bound, char **ptr);

void step_backward_to_separator(const char *bound, char **ptr);

void align_pointer_on_separator(const char *lower_bound,
                                const char *upper_bound,int offset,
                                char **ptr);

void align_pointers_on_separator(const char *lower_bound,
                                 const char *upper_bound, int lower_extend,
                                 int upper_extend, char **head, char **tail);

long get_first_line_length(char *buf);

int compare_long(const long *l1, const long *l2);
long long gcd(long long a, long long b);

FILE *fopen_mkdir(const char *path, const char *mode);

long pad_input(char *input_buffer, char **padded_buffer_out,
                     long filesize,char filler);
#endif // AOCC_HELPERS_H
