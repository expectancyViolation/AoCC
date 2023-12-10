//
// Created by matze on 10/12/2023.
//

#ifndef AOCC_DAY10_H
#define AOCC_DAY10_H

#include "../util/aoc_types.h"

#include "../util/helpers.h"
#include "../util/ll_tuple.h"
#include "../util/parallelize.h"

#define LEFT_HAND true

enum day10_facing {
  facing_none = 0,
  facing_north = 1,
  facing_east = 2,
  facing_south = 3,
  facing_west = 4
};

void day10_print_facing(enum day10_facing facing) {
  switch (facing) {
  case facing_none:
    printf("noner");
    break;
  case facing_north:
    printf("north");
    break;
  case facing_east:
    printf("east");
    break;
  case facing_south:
    printf("south");
    break;
  case facing_west:
    printf("west");
    break;
  default:
    printf("???");
  }
}

const enum day10_facing DAY10_ALL_FACING[] = {facing_north, facing_east,
                                              facing_south, facing_west};

const size_t DAY10_LEN_ALL_FACING =
    sizeof(DAY10_ALL_FACING) / sizeof(facing_none);

struct day10_piece {
  enum day10_facing north_connect;
  enum day10_facing east_connect;
  enum day10_facing south_connect;
  enum day10_facing west_connect;
};

const struct day10_piece V_PIPE = {facing_north, facing_none, facing_south,
                                   facing_none};
const struct day10_piece H_PIPE = {facing_none, facing_east, facing_none,
                                   facing_west};
const struct day10_piece L_BEND = {facing_none, facing_none, facing_east,
                                   facing_north};
const struct day10_piece J_BEND = {facing_none, facing_north, facing_west,
                                   facing_none};
const struct day10_piece SEVEN_BEND = {facing_west, facing_south, facing_none,
                                       facing_none};
const struct day10_piece F_BEND = {facing_east, facing_none, facing_none,
                                   facing_south};
const struct day10_piece GROUND = {facing_none, facing_none, facing_none,
                                   facing_none};

struct day10_move_state {
  long x;
  long y;
  enum day10_facing facing;
  bool left_hand;
};

bool day10_at_same_pos(const struct day10_move_state *s1,
                       const struct day10_move_state *s2) {
  return (s1->x == s2->x) && (s1->y == s2->y);
}

void day10_print_move_state(const struct day10_move_state *state) {
  assert(state != NULL);
  printf("move state:\n"
         "\tx:%10ld\n"
         "\tx:%10ld\n"
         "\tleft: %d\n\t",
         state->x, state->y, state->left_hand);
  day10_print_facing(state->facing);
  printf("\n\n");
}

void day10_direction_to_delta(enum day10_facing dir, long *dx, long *dy) {
  switch (dir) {
  case facing_north:
    *dx = -1;
    *dy = 0;
    break;
  case facing_east:
    *dx = 0;
    *dy = 1;
    break;
  case facing_south:
    *dx = 1;
    *dy = 0;
    break;
  case facing_west:
    *dx = 0;
    *dy = -1;
    break;
  case facing_none:
    *dx = 0;
    *dy = 0;
  }
}

const struct day10_piece *get_tile_piece(char tile) {
  switch (tile) {
  case '|':
    return &V_PIPE;
  case '-':
    return &H_PIPE;
  case 'L':
    return &L_BEND;
  case 'J':
    return &J_BEND;
  case '7':
    return &SEVEN_BEND;
  case 'F':
    return &F_BEND;
  case '.':
    return &GROUND;
  default:
    printf("%x is ground\n", tile);
    // we are surrounded by ground
    return &GROUND;
  }
}

enum day10_facing step_tile(char tile, enum day10_facing dir) {
  const struct day10_piece *curr_piece = get_tile_piece(tile);
  switch (dir) {
  case facing_none:
    return facing_none;
  case facing_north:
    return curr_piece->north_connect;
  case facing_east:
    return curr_piece->east_connect;
  case facing_south:
    return curr_piece->south_connect;
  case facing_west:
    return curr_piece->west_connect;
  }
}

bool day10_step(const char *buf, long line_length,
                struct day10_move_state *state, struct ll_tuple **boundary) {
  long dx = 0;
  long dy = 0;
  enum day10_facing facing_old = state->facing;
  day10_direction_to_delta(state->facing, &dx, &dy);
  long curr_pos_offset = (state->x) * line_length + state->y;
  long next_pos_offset = curr_pos_offset + dx * line_length + dy;
  char next_tile = buf[next_pos_offset];
  enum day10_facing next_facing = step_tile(next_tile, state->facing);
  state->facing = next_facing;
  // check if we moved
  bool moved = state->facing != facing_none;
  if (moved) {
    // boundary calculation
    struct ll_tuple old_boundary = {};
    old_boundary.left =
        2 * (state->x) + dx - ((state->left_hand) ? (dy) : (-dy));
    old_boundary.right =
        2 * (state->y) + dy + ((state->left_hand) ? (dx) : (-dx));
    // step
    state->x += dx;
    state->y += dy;
    day10_direction_to_delta(state->facing, &dx, &dy);

    struct ll_tuple new_boundary = {};
    new_boundary.left =
        2 * (state->x) + dx - ((state->left_hand) ? (dy) : (-dy));
    new_boundary.right =
        2 * (state->y) + dy + ((state->left_hand) ? (dx) : (-dx));

    const long long boundary_distance =
        llabs(new_boundary.left - old_boundary.left) +
        llabs(new_boundary.right - old_boundary.right);

    struct ll_tuple intermediary_boundary = {};
    switch (boundary_distance) {
    case 0:
      // "inner" corner => no new boundary point (e.g. when "LEFT HAND" and we
      // turn left)
      break;
    case 2:
      // straight move: just add new boundary
      cvector_push_back(*boundary, new_boundary);
      break;
    case 4:
      // "outer" corner => step new boundary 2*(dx,dy ) backwards
      intermediary_boundary.left = new_boundary.left - 2 * dx;
      intermediary_boundary.right = new_boundary.right - 2 * dy;
      cvector_push_back(*boundary, intermediary_boundary);
      cvector_push_back(*boundary, new_boundary);
      break;
    default:
      assert(false);
    }

    printf("moved:\n");
    day10_print_move_state(state);

    printf("old boundary:\n");
    print_tuple(old_boundary);
    printf("\n");

    printf("new boundary:\n");
    print_tuple(new_boundary);
    printf("\n");
  }
  return moved;
}

void day10_print_enlarged(char *buf, long line_length,
                          const struct ll_tuple *boundary) {
  const int buf_len = strlen(buf) + 1;
  const int new_buf_len=4*buf_len;
  char *out = malloc(4 * buf_len + 4);
  memset(out, ' ', 4 * buf_len + 4);
  const int new_line_length=2*line_length-1;
  for (int i = 0; i < buf_len; i++) {
    const long line_num = i / (line_length-1);
    const long line_pos = i % (line_length-1);
    out[line_num * 4 * line_length + 2 * line_pos+1] = buf[i];
  }
  for(int i=0;i<new_buf_len;i+=new_line_length){
    out[i]='\n';
  }
  for (int i = 0; i < cvector_size(boundary); i++) {
    const struct ll_tuple b_el = boundary[i];
    print_tuple(b_el);
    const size_t mark_offset =
        (b_el.left) * new_line_length + (b_el.right)+1;
    out[mark_offset] = 'X';
  }
  printf("%s", out);
}

struct ll_tuple day10(char *buf, __attribute__((unused)) long buf_len) {
  printf("----\n%s\n", buf);
  struct ll_tuple res = {};
  struct day10_move_state current_states[2] = {};
  struct ll_tuple *boundary = NULL;

  const long line_length = strchr(buf, '\n') - buf + 1;
  printf("line length %ld\n", line_length);
  ptrdiff_t curr_offset = strchr(buf, 'S') - buf;

  int j = 0;
  for (int i = 0; i < DAY10_LEN_ALL_FACING; i++) {
    struct day10_move_state temp_state = {};
    temp_state.x = curr_offset / line_length;
    temp_state.y = curr_offset % line_length;
    temp_state.facing = DAY10_ALL_FACING[i];
    temp_state.left_hand = j;
    if (day10_step(buf, line_length, &temp_state, &boundary)) {
      memcpy(&current_states[j++], &temp_state,
             sizeof(struct day10_move_state));
    }
  }
  assert(j == 2);

  for (size_t i = 0; i < 2; i++) {
    day10_print_move_state(&current_states[i]);
  }

  int n_steps = 1;
  while (!day10_at_same_pos(current_states, current_states + 1)) {
    day10_step(buf, line_length, current_states, &boundary);
    day10_step(buf, line_length, current_states + 1, &boundary);
    n_steps++;
    assert(current_states[0].facing != facing_none);
    assert(current_states[1].facing != facing_none);
  }

  //  for (size_t i = 0; i < 2; i++) {
  //    day10_print_move_state(&current_states[i]);
  //  }
  //  printf("met at %d\n", n_steps);
  printf("boundary is:\n");
  for (int i = 0; i < cvector_size(boundary); i++) {
    print_tuple(boundary[i]);
    printf("\n");
  }
  printf("%s\n", buf);
  day10_print_enlarged(buf, line_length, boundary);
  res.left = n_steps;
  return res;
}

struct aoc_day_res solve_day10(const char *input_file) {
  char *input_buffer;
  char *padded_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, false);
  const long padded_buffer_len =
      pad_input(input_buffer, &padded_buffer, filesize, '.');
  const struct ll_tuple res = day10(padded_buffer, padded_buffer_len);
  struct aoc_day_res day_res = {res};
  free(padded_buffer);
  free(input_buffer);
  return day_res;
}

#endif // AOCC_DAY10_H
