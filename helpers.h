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

struct two_part_result {
  long long part1_result;
  long long part2_result;
};

struct two_part_result *allocate_two_part_result() {
  struct two_part_result *result = malloc(sizeof *result);
  result->part1_result = 0;
  result->part2_result = 0;
  return result;
}

void add_consume_partial_result(struct two_part_result *result,
                                struct two_part_result *partial_result) {
  (result->part2_result) += (partial_result->part2_result);
  (result->part1_result) += (partial_result->part1_result);
  free(partial_result);
}

void print_two_part_result(struct two_part_result *result) {
  printf("result:\npart1:\t%lld\npart2:\t%lld\n", result->part1_result,
         result->part2_result);
}

void print_day_result(char *day, struct two_part_result *result) {
  printf("----------\n");
  printf("%s\n", day);
  printf("---\n");
  print_two_part_result(result);
  printf("----------\n\n");
}

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
