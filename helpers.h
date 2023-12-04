#ifndef AOCC_HELPERS_H
#define AOCC_HELPERS_H

#include <malloc.h>

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

// DANGER: free string after use
long read_file_to_memory(const char *filename, char **file_content_out,
                         bool append_newline) {
  FILE *f = fopen(filename, "rb");
  fseek(f, -1, SEEK_END);
  long fsize = ftell(f) + 1;
  char last_char = (char)fgetc(f);
  fseek(f, 0, SEEK_SET);
  bool add_newline = append_newline && (last_char != '\n');
  *file_content_out = malloc(fsize + 1 + add_newline);
  fread(*file_content_out, fsize, 1, f);
  fclose(f);

  if (add_newline)
    (*file_content_out)[fsize + add_newline - 1] = '\n';

  (*file_content_out)[fsize + add_newline] = 0;
  return fsize + 1 + add_newline;
}

void step_forward_to_separator(const char *bound, char **ptr) {
  while ((**ptr != '\n') && (**ptr != 0) && (*ptr != bound)) {
    ++*ptr;
  }
}

void step_backward_to_separator(const char *bound, char **ptr) {
  while ((**ptr != '\n') && (**ptr != 0) && (*ptr != bound)) {
    --*ptr;
  }
}

void align_pointer_on_separator(const char *lower_bound,
                                const char *upper_bound, const int offset,
                                char **ptr) {
  // printf("aligning (%x-%x) with offset %d starting from
  // %x\n",lower_bound,upper_bound,offset,*ptr);
  step_forward_to_separator(upper_bound, ptr);
  if (offset >= 0) {
    for (int i = 0; i < offset; i++) {
      ++*ptr;
      step_forward_to_separator(upper_bound, ptr);
    }
  } else {
    step_backward_to_separator(lower_bound, ptr);
    for (int i = 0; i > offset; i--) {
      --*ptr;
      step_backward_to_separator(lower_bound, ptr);
    }
  }
  // printf("aligned: %x\n",*ptr);
}

void align_pointers_on_separator(const char *lower_bound,
                                 const char *upper_bound, int lower_extend,
                                 int upper_extend, char **head, char **tail) {
  align_pointer_on_separator(lower_bound, upper_bound, -lower_extend, head);
  align_pointer_on_separator(lower_bound, upper_bound, upper_extend, tail);
}

long get_first_line_length(char *buf) { return strchr(buf, '\n') - buf + 1; }

long compare_long(long *l1, long *l2) { return (*l1) - (*l2); }

#endif // AOCC_HELPERS_H
