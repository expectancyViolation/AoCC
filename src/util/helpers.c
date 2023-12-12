#include "helpers.h"

long read_file_to_memory(const char *filename, char **file_content_out,
                         bool strip_newline) {
  FILE *f = fopen(filename, "rb");
  fseek(f, -1, SEEK_END);
  long fsize = ftell(f) + 1;
  char last_char = (char)fgetc(f);
  fseek(f, 0, SEEK_SET);
  bool remove_newline = strip_newline && (last_char == '\n');
  *file_content_out = malloc(fsize + 1 - remove_newline);
  fread(*file_content_out, fsize - remove_newline, 1, f);

  fclose(f);

  (*file_content_out)[fsize - remove_newline] = 0;
  return fsize + 1 - remove_newline;
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
  // %x\n",num_solution_lower_bound,num_solution_upper_bound,offset,*ptr);
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

int compare_long(const long *l1, const long *l2) { return (*l1) - (*l2); }

// from: https://stackoverflow.com/questions/19738919/gcd-function-for-c
long long gcd(long long a, long long b) {
  if (a && b)
    for (; (a %= b) && (b %= a);)
      ;
  return a | b;
}
FILE *fopen_mkdir(const char *path, const char *mode) {
  char *p = strdup(path);
  char *sep = strchr(p + 1, '/');
  while (sep != NULL) {
    *sep = '\0';
    if (mkdir(p, 0755) && errno != EEXIST) {
      fprintf(stderr, "error while trying to create %s\n", p);
    }
    *sep = '/';
    sep = strchr(sep + 1, '/');
  }
  free(p);
  return fopen(path, mode);
}

void place_pad_line(char **buf_ptr, long line_length, char filler) {
  memset(*buf_ptr, filler, line_length);
  *(*buf_ptr + line_length - 1) = '\n';
  *buf_ptr += line_length;
}

long pad_input(char *input_buffer, char **padded_buffer_out, long filesize,
               char filler) {
  const long line_length = get_first_line_length(input_buffer);
  const long padded_buffer_len = filesize + 4 * (line_length)-1;
  *padded_buffer_out = malloc(padded_buffer_len);
  char *padded_buffer = *padded_buffer_out;
  char *pad_ptr = padded_buffer;

  place_pad_line(&pad_ptr, line_length, filler);
  place_pad_line(&pad_ptr, line_length, filler);

  strncpy(pad_ptr, input_buffer, filesize - 1);
  pad_ptr += filesize - 1;

  place_pad_line(&pad_ptr, line_length, filler);
  place_pad_line(&pad_ptr, line_length, filler);

  *(padded_buffer + padded_buffer_len - 1) = 0;
  return padded_buffer_len;
}

#ifdef WIN32
char *strsep(char **stringp, const char *delim) {
  char *start = *stringp;
  char *p;

  p = (start != NULL) ? strpbrk(start, delim) : NULL;

  if (p == NULL) {
    *stringp = NULL;
  } else {
    *p = '\0';
    *stringp = p + 1;
  }

  return start;
}
#endif
