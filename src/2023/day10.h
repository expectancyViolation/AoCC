//
// Created by matze on 10/12/2023.
//

#ifndef AOCC_DAY10_H
#define AOCC_DAY10_H

#include "../util/aoc_types.h"

#include "../util/helpers.h"
#include "../util/ll_tuple.h"
#include "../util/parallelize.h"

enum day10_facing {
  facing_none = 0,
  facing_north = 1,
  facing_east = 2,
  facing_south = 3,
  facing_west = 4
};

const enum day10_facing DAY10_ALL_FACINGS[]={facing_north,facing_east,facing_south,facing_west};

const size_t DAY10_FACINGS_COUNT=sizeof(DAY10_ALL_FACINGS)/sizeof(facing_none);


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

struct day10_map {
  const char *buf;
  long buf_len;
  long x_extent;
  long y_extent;
  struct ll_tuple start_position;
};

struct day10_map *day10_map_initialize(const char *buf, long buf_len) {
  struct day10_map *map = malloc(sizeof(*map));
  map->buf = buf;
  map->buf_len=buf_len;
  map->y_extent = (strchr(buf, '\n') - buf);
  map->x_extent = buf_len / (map->y_extent + 1);
  ptrdiff_t start_offset = strchr(map->buf, 'S') - map->buf;
  map->start_position.left = start_offset / (map->y_extent + 1);
  map->start_position.right = start_offset % (map->y_extent + 1);
  return map;
}

__attribute__((unused)) void day10_print_map(const struct day10_map *map) {
  printf("%s\n", map->buf);
  printf("%ld by %ld\n", map->x_extent, map->y_extent);
}

char day10_lookup_tile(const struct day10_map *map,
                       const struct ll_tuple *position) {
  long curr_pos_offset =
      (position->left) * (map->y_extent + 1) + position->right;
  if((curr_pos_offset<0)|| (curr_pos_offset>=map->buf_len))
    return '.';
  return map->buf[curr_pos_offset];
}

struct day10_move_state {
  struct ll_tuple position;
  enum day10_facing facing;
};

void day10_step_direction(enum day10_facing dir, struct ll_tuple *position) {
  switch (dir) {
  case facing_north:
    position->left += -1;
    break;
  case facing_east:
    position->right += 1;
    break;
  case facing_south:
    position->left += 1;
    break;
  case facing_west:
    position->right -= 1;
    break;
  default:
    break;
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

enum day10_facing get_new_facing(char tile, enum day10_facing dir) {
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
  return facing_none;
}

bool day10_step(const struct day10_map *map, struct day10_move_state *state) {
  struct ll_tuple next_position = {state->position.left, state->position.right};
  day10_step_direction(state->facing, &next_position);
  char next_tile = day10_lookup_tile(map, &next_position);
  enum day10_facing next_facing = get_new_facing(next_tile, state->facing);
  state->facing = next_facing;
  // check if we moved
  bool moved = state->facing != facing_none;
  if (moved) {
    // step
    state->position.left = next_position.left;
    state->position.right = next_position.right;
  }
  return moved;
}

long long signed_area(const struct ll_tuple *t1, const struct ll_tuple *t2,
                      const struct ll_tuple *t3) {
  const long long v1x = (t2->left) - (t1->left);
  const long long v1y = (t2->right) - (t1->right);
  const long long v2x = (t3->left) - (t1->left);
  const long long v2y = (t3->right) - (t1->right);
  return v1x * v2y - v2x * v1y;
}

struct ll_tuple day10(char *buf, long buf_len) {
  struct ll_tuple res = {};
  const struct day10_map *map = day10_map_initialize(buf, buf_len);

  //day10_print_map(map);

  struct day10_move_state current_state = {
      {map->start_position.left, map->start_position.right}, facing_none};
  for (size_t i = 0; i < DAY10_FACINGS_COUNT; i++) {
    current_state.facing = DAY10_ALL_FACINGS[i];
    if (day10_step(map, &current_state))
      break;
  }

  struct ll_tuple previous_position = {current_state.position.left,
                                       current_state.position.right};

  long long boundary_size = 1;
  long long twice_interior_size = 0;

  twice_interior_size += signed_area(&map->start_position, &previous_position,
                                     &map->start_position);

  while (true) {
    day10_step(map, &current_state);
    boundary_size += 1;

    if (current_state.facing == facing_none) {
      // reached the beginning (if everything went right)
      break;
    }
    // calculate interior
    twice_interior_size += signed_area(&map->start_position, &previous_position,
                                       &current_state.position);
    previous_position.left = current_state.position.left;
    previous_position.right = current_state.position.right;
  }

  assert((boundary_size % 2) == 0);
  assert((twice_interior_size % 2) == 0);

  res.left = boundary_size / 2;
  res.right = (llabs(twice_interior_size) - boundary_size) / 2 + 1;

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
