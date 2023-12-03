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
    (*file_content_out)[fsize - 1] = '\n';

  (*file_content_out)[fsize] = 0;
  return fsize;
}

void align_pointers_on_separator(const char *buf, char **head, char **tail) {
  if (*head != buf) {
    while ((**head != '\n') && (**head != 0))
      ++*head;
    ++*head;
  }
  while (**tail != '\n' && (**tail != 0))
    ++*tail;
}

#endif // AOCC_HELPERS_H
