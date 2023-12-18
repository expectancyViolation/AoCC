#include "day18.h"

static long long signed_area(const LLTuple *t1, const LLTuple *t2,
                             const LLTuple *t3) {
  const long long v1x = (t2->left) - (t1->left);
  const long long v1y = (t2->right) - (t1->right);
  const long long v2x = (t3->left) - (t1->left);
  const long long v2y = (t3->right) - (t1->right);
  return v1x * v2y - v2x * v1y;
}

LLTuple lltuple_add(const LLTuple t1, const LLTuple t2) {
  return (LLTuple){t1.left + t2.left, t1.right + t2.right};
}

LLTuple lltuple_scale(const LLTuple t, long long factor) {
  return (LLTuple){factor * t.left, factor * t.right};
}

static LLTuple parse_direction_p1(char d) {
  switch (d) {
  case 'U':
    return (LLTuple){-1, 0};
  case 'R':
    return (LLTuple){0, 1};
  case 'D':
    return (LLTuple){1, 0};
  case 'L':
    return (LLTuple){0, -1};
  }
}

static LLTuple parse_direction_p2(char d) {
  switch (d) {
  case '3':
    return (LLTuple){-1, 0};
  case '0':
    return (LLTuple){0, 1};
  case '1':
    return (LLTuple){1, 0};
  case '2':
    return (LLTuple){0, -1};
  }
}

LLTuple year23_day18(char *buf, long buf_len) {
  LLTuple res = {0};

  LLTuple curr_state_p1 = {0, 0};
  LLTuple curr_state_p2 = {0, 0};

  long long twice_interior_size_p1 = 0;
  long long boundary_size_p1 = 0;

  long long twice_interior_size_p2 = 0;
  long long boundary_size_p2 = 0;

  while (buf != NULL) {

    // parse p1
    char *line = strsep(&buf, "\n");
    const LLTuple delta_p1 = parse_direction_p1(line[0]);
    const long step_count_p1 = strtol(line + 1, NULL, 10);

    // solve p1
    const LLTuple new_state_p1 =
        lltuple_add(curr_state_p1, lltuple_scale(delta_p1, step_count_p1));
    boundary_size_p1 += step_count_p1;

    twice_interior_size_p1 +=
        signed_area(&(LLTuple){0, 0}, &curr_state_p1, &new_state_p1);
    curr_state_p1 = new_state_p1;

    // parse p2
    strsep(&line, "(");
    const LLTuple delta_p2 = parse_direction_p2(line[6]);
    line[6] = 0;
    const long step_count_p2 = strtol(line + 1, NULL, 16);

    // solve p2
    const LLTuple new_state_p2 =
        lltuple_add(curr_state_p2, lltuple_scale(delta_p2, step_count_p2));
    boundary_size_p2 += step_count_p2;

    twice_interior_size_p2 +=
        signed_area(&(LLTuple){0, 0}, &curr_state_p2, &new_state_p2);
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
