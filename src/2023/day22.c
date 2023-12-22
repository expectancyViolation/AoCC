#include "day22.h"

#define LEAVEOUT_NONE (-1)

enum AXIS { AXIS_undefined = 0, AXIS_x = 1, AXIS_y = 2, AXIS_z = 3 };

typedef struct {
  long x;
  long y;
  long z;
} D22Position;

static const D22Position AXES_DELTAS[4] = {
    {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

static D22Position add_scaled(D22Position const pos, D22Position const add,
                              long scale) {
  return (D22Position){.x = pos.x + scale * add.x,
                       .y = pos.y + scale * add.y,
                       .z = pos.z + scale * add.z};
}

typedef struct {
  D22Position position;
  enum AXIS axis;
  long length;
} D22Brick;

D22Position d22_brick_lookup(D22Brick const *brick, int pos) {
  assert((0 <= pos) && (pos < brick->length));
  assert(brick->axis != AXIS_undefined);
  return add_scaled(brick->position, AXES_DELTAS[brick->axis], pos);
}

static int d22brick_z_compare(const void *a, const void *b) {
  const D22Brick *sa = a;
  const D22Brick *sb = b;
  return CMP(sa->position.z, sb->position.z);
}

D22Position parse_position(char *string) {
  long x = strtol(string, &string, 10);
  string++;
  long y = strtol(string, &string, 10);
  string++;
  long z = strtol(string, &string, 10);
  return (D22Position){.x = x, .y = y, .z = z};
}

D22Brick parse_brick(char *line) {
  char *brick_begin = strsep(&line, "~");
  D22Position start_pos = parse_position(brick_begin);
  D22Position end_pos = parse_position(line);
  enum AXIS axis = AXIS_x;
  long length = 1;
  if (end_pos.x != start_pos.x) {
    axis = AXIS_x;
    length = end_pos.x - start_pos.x + 1;
  }
  if (end_pos.y != start_pos.y) {
    axis = AXIS_y;
    length = end_pos.y - start_pos.y + 1;
  }
  if (end_pos.z != start_pos.z) {
    axis = AXIS_z;
    length = end_pos.z - start_pos.z + 1;
  }

  return (D22Brick){.position = start_pos, .axis = axis, .length = length};
}

int fall_down(D22Brick *bricks_vec, long x_size, long y_size,
              bool simulate_only, int leave_out) {
  int fell = 0;

  const size_t len_heights = ((long)sizeof(long)) * x_size * y_size;
  long *heights = malloc(len_heights);

  for (int i = 0; i < x_size * y_size; i++)
    heights[i] = 0;

  for (int j = 0; j < cvector_size(bricks_vec); j++) {
    if (j == leave_out)
      continue;
    D22Brick *curr_brick = &(bricks_vec[j]);
    long fall_pos = 0;
    // scan heightmap for fall position
    for (int i = 0; i < curr_brick->length; i++) {
      const D22Position part_pos = d22_brick_lookup(curr_brick, i);
      const long pos_height = heights[part_pos.x * y_size + part_pos.y];
      fall_pos = max(fall_pos, pos_height);
    }
    bool curr_changed = (curr_brick->position.z) != (fall_pos + 1);

    fell += curr_changed;

    // set brick down
    int fall_height = (curr_brick->position.z) - fall_pos - 1;
    for (int i = 0; i < curr_brick->length; i++) {
      const D22Position part_pos = d22_brick_lookup(curr_brick, i);
      heights[part_pos.x * y_size + part_pos.y] = part_pos.z - fall_height;
    }

    if (!simulate_only)
      curr_brick->position.z = fall_pos + 1;
  }
  free(heights);
  return fell;
}

LLTuple year23_day22(char *buf, long buf_len) {
  LLTuple res = {0};
  D22Brick *bricks_vec = NULL;
  long max_x = 0;
  long max_y = 0;
  long max_len = 0;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    const D22Brick curr_brick = parse_brick(line);
    cvector_push_back(bricks_vec, curr_brick);
    max_x = max(max_x, curr_brick.position.x);
    max_y = max(max_y, curr_brick.position.y);
    max_len = max(max_len, curr_brick.length);
  }

  const long x_size = max_x + max_len + 1;
  const long y_size = max_y + max_len + 1;

  qsort(bricks_vec, cvector_size(bricks_vec), sizeof(D22Brick),
        d22brick_z_compare);
  fall_down(bricks_vec, x_size, y_size, false, LEAVEOUT_NONE);

  qsort(bricks_vec, cvector_size(bricks_vec), sizeof(D22Brick),
        d22brick_z_compare);

  bool *safe_results = malloc(sizeof(bool) * cvector_size(bricks_vec));
  int *disintegrate_results = malloc(sizeof(int) * cvector_size(bricks_vec));
#pragma omp parallel for
  for (int leave_out = 0; leave_out < cvector_size(bricks_vec); leave_out++) {
    int fell = fall_down(bricks_vec, x_size, y_size, true, leave_out);
    safe_results[leave_out] = (fell == 0);
    disintegrate_results[leave_out] = fell;
  }

  for (int leave_out = 0; leave_out < cvector_size(bricks_vec); leave_out++) {
    res.left += safe_results[leave_out];
    res.right += disintegrate_results[leave_out];
  }

  free(disintegrate_results);
  free(safe_results);
  cvector_free(bricks_vec);
  return res;
}
AocDayRes solve_year23_day22(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day22(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
