#include "day18.h"

typedef struct {
  int x;
  int y;
} D18State;

static long long signed_area(const D18State *t1, const D18State *t2,
                             const D18State *t3) {
  const long long v1x = (t2->x) - (t1->x);
  const long long v1y = (t2->y) - (t1->y);
  const long long v2x = (t3->x) - (t1->x);
  const long long v2y = (t3->y) - (t1->y);
  return v1x * v2y - v2x * v1y;
}

static LLTuple parse_direction_p1(char d) {
  LLTuple delta = {0, 0};
  switch (d) {
  case 'U':
    delta.left = -1;
    break;
  case 'R':
    delta.right = 1;
    break;
  case 'D':
    delta.left = 1;
    break;
  case 'L':
    delta.right = -1;
    break;
  }
  return delta;
}

static LLTuple parse_direction_p2(char d) {
  LLTuple delta = {0, 0};
  switch (d) {
  case '3':
    delta.left = -1;
    break;
  case '0':
    delta.right = 1;
    break;
  case '1':
    delta.left = 1;
    break;
  case '2':
    delta.right = -1;
    break;
  }
  return delta;
}

LLTuple year23_day18(char *buf, long buf_len) {
  LLTuple res = {0};

  D18State curr_state_p1 = {0, 0};
  D18State curr_state_p2 = {0, 0};

  long long twice_interior_size_p1 = 0;
  long long boundary_size_p1 = 0;

  long long twice_interior_size_p2 = 0;
  long long boundary_size_p2 = 0;

  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    const LLTuple delta_p1 = parse_direction_p1(line[0]);
    const long step_count_p1 = strtol(line + 1, NULL, 10);

    const D18State new_state_p1 = {
        curr_state_p1.x + step_count_p1 * delta_p1.left,
        curr_state_p1.y + step_count_p1 * delta_p1.right};
    boundary_size_p1 += step_count_p1;

    twice_interior_size_p1 +=
        signed_area(&(D18State){0, 0}, &curr_state_p1, &new_state_p1);
    curr_state_p1 = new_state_p1;

    strsep(&line, "(");
    printf("%s\n", line);

    const LLTuple delta_p2 = parse_direction_p2(line[6]);

    line[6] = 0;

    const long step_count_p2 = strtol(line + 1, NULL, 16);
    const D18State new_state_p2 = {
        curr_state_p2.x + step_count_p2 * delta_p2.left,
        curr_state_p2.y + step_count_p2 * delta_p2.right};
    boundary_size_p2 += step_count_p2;

    twice_interior_size_p2 +=
        signed_area(&(D18State){0, 0}, &curr_state_p2, &new_state_p2);
    curr_state_p2 = new_state_p2;
  }
  res.left = (llabs(twice_interior_size_p1) + boundary_size_p1) / 2 + 1;
  res.right = (llabs(twice_interior_size_p2) + boundary_size_p2) / 2 + 1;
  return res;
}
AocDayRes solve_year23_day18(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day18(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
