#include "day16.h"
#include "../../res/hashmap.c/hashmap.h"
#include <bits/stdint-uintn.h>

enum FACING {
  FACING_north = 1 << 0,
  FACING_east = 1 << 1,
  FACING_south = 1 << 2,
  FACING_west = 1 << 3
};

#define NUM_FACING 4

static int facing_index(enum FACING facing) {
  switch (facing) {
  case FACING_north:
    return 0;
  case FACING_east:
    return 1;
  case FACING_south:
    return 2;
  case FACING_west:
    return 3;
  }
}

static LLTuple facing_to_delta(enum FACING facing) {
  LLTuple res = {0};
  switch (facing) {
  case FACING_north:
    res.left -= 1;
    break;
  case FACING_east:
    res.right += 1;
    break;
  case FACING_south:
    res.left += 1;
    break;
  case FACING_west:
    res.right -= 1;
    break;
  }
  return res;
}

typedef struct {
  long long x;
  long long y;
  enum FACING facing;
} D16State;

void print_d16state(const D16State *state) {
  printf("state:(%lld,%lld,%d)\n", state->x, state->y, state->facing);
}

enum D16TILE {
  D16TILE_empty_space = 0,
  D16TILE_mirror_up = 1,
  D16TILE_mirror_down = 2,
  D16TILE_splitter_vertical = 3,
  D16TILE_splitter_horizontal = 4,
  D16TILE_outside = 5 // outside of map
};

#define NUM_D16TILE 6

int tile_facing_lookup[NUM_D16TILE][NUM_FACING] = {
    {// empty_space
     FACING_north, FACING_east, FACING_south, FACING_west},
    {// mirror_up     (n<->e)  and (s<->w)
     FACING_east, FACING_north, FACING_west, FACING_south},
    {// mirror_down   (n<->w) and (s<->e)
     FACING_west, FACING_south, FACING_east, FACING_north},
    {// splitter_vertical
     FACING_north, FACING_north + FACING_south, FACING_south,
     FACING_north + FACING_south},
    {// splitter_horizontal
     FACING_east + FACING_west, FACING_east, FACING_east + FACING_west,
     FACING_west},
    {// outside
     0, 0, 0, 0}};

enum D16TILE parse_d16_tile(char c) {
  switch (c) {
  case '.':
    return D16TILE_empty_space;
  case '/':
    return D16TILE_mirror_up;
  case '\\':
    return D16TILE_mirror_down;
  case '|':
    return D16TILE_splitter_vertical;
  case '-':
    return D16TILE_splitter_horizontal;
  default:
    assert(false);
  }
}

typedef struct {
  enum D16TILE *map;
  int x_size;
  int y_size;
} D16Map;

void d16map_set(const D16Map *map, int x, int y, enum D16TILE tile) {
  map->map[map->y_size * x + y] = tile;
}

enum D16TILE d16map_lookup(const D16Map *map, int x, int y) {
  if ((0 <= x) && (x < map->x_size) && (0 <= y) && (y < map->y_size))
    return map->map[(map->y_size) * x + y];
  return D16TILE_outside;
}

void d16map_print(const D16Map *map) {
  printf("map size: %d by %d:\n", map->x_size, map->y_size);
  for (int i = 0; i < map->x_size; i++) {
    for (int j = 0; j < map->y_size; j++) {
      printf("%hhd", d16map_lookup(map, i, j));
    }
    printf("\n");
  }
}

int d16state_compare(const void *a, const void *b, void *udata) {
  const D16State *sa = a;
  const D16State *sb = b;
  const int x_cmp = CMP(sa->x, sb->x);
  if (x_cmp != 0)
    return x_cmp;
  const int y_cmp = CMP(sa->y, sb->y);
  if (y_cmp != 0)
    return y_cmp;
  return CMP(sa->facing, sb->facing);
}

u_int64_t d16state_hash(const void *item, u_int64_t seed0, u_int64_t seed1) {
  const D16State *res = item;
  uint64_t hash_val = hashmap_sip(res, (sizeof(*res)), 0, 0);
  return hash_val;
}

// returns resulting facings
int interact_facing(enum FACING facing, enum D16TILE tile) {
  return tile_facing_lookup[tile][facing_index(facing)];
}

static void interact(D16State state, const D16Map *map, struct hashmap *out_states) {
  const enum D16TILE curr_tile = d16map_lookup(map, state.x, state.y);
  int new_facing_flags = interact_facing(state.facing, curr_tile);
  for (int facing_index = 0; facing_index < NUM_FACING; facing_index++) {
    enum FACING curr_facing = 1 << facing_index;
    if (curr_facing & new_facing_flags) {
      state.facing = curr_facing;
      hashmap_set(out_states, &state);
    }
  }
}

// moves state 1 step
static D16State step(D16State state) {
  LLTuple delta = facing_to_delta(state.facing);
  state.x += delta.left;
  state.y += delta.right;
  return state;
}

static long long solve(const D16Map *map, D16State initial_state) {
  struct hashmap *states = hashmap_new(sizeof(D16State), 0, 0, 0, d16state_hash,
                                       d16state_compare, NULL, NULL);

  struct hashmap *visited_states = hashmap_new(
      sizeof(D16State), 0, 0, 0, d16state_hash, d16state_compare, NULL, NULL);

  // start right off the map

  hashmap_set(states, &initial_state);

  while (hashmap_count(states) > 0) {
    struct hashmap *new_states = hashmap_new(
        sizeof(D16State), 0, 0, 0, d16state_hash, d16state_compare, NULL, NULL);
    size_t iter = 0;
    void *item;
    while (hashmap_iter(states, &iter, &item)) {
      D16State *state = item;
      if (hashmap_get(visited_states, state) == NULL) {
        hashmap_set(visited_states, state);
        D16State new_state = step(*state);
        interact(new_state, map, new_states);
      }
    }
    hashmap_free(states);
    states = new_states;
    // printf("frontier: %ld\n", hashmap_count(states));
    // printf("visited: %ld\n", hashmap_count(visited_states));
  }

  // count by turning all states north (saves on defining a new (x,y) hashmap)
  struct hashmap *new_states = hashmap_new(
      sizeof(D16State), 0, 0, 0, d16state_hash, d16state_compare, NULL, NULL);
  size_t iter = 0;
  void *item;
  while (hashmap_iter(visited_states, &iter, &item)) {
    D16State *state = item;
    D16State north_state = *state;
    north_state.facing = FACING_north;
    hashmap_set(new_states, &north_state);
  }
  return hashmap_count(new_states) - 1; // do not count initial outside state
}

static void fill_map(D16Map *map, char *buf, long buf_len) {
  const int y_size = strchr(buf, '\n') - buf;
  const int x_size = buf_len / (y_size + 1);

  map->x_size = x_size;
  map->y_size = y_size;
  map->map = malloc(sizeof(*map->map) * map->x_size * map->y_size);
  int curr_x = 0;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    printf("%s\n", line);
    for (int i = 0; i < map->y_size; i++) {
      d16map_set(map, curr_x, i, parse_d16_tile(line[i]));
    }
    curr_x++;
  }
}

LLTuple year23_day16(char *buf, long buf_len) {
  LLTuple res = {0};
  D16Map map = {0};
  fill_map(&map, buf, buf_len);
  D16State initial_state = {.x = 0, .y = -1, .facing = FACING_east};
  res.left = solve(&map, initial_state);

  long long curr_best = 0;
  for (int i = 0; i < 4; i++) {
    enum FACING facing = 1 << i;
    LLTuple delta = facing_to_delta(facing);
    const long long dx = delta.left;
    const long long dy = delta.right;
    const long long step_x = dy;
    const long long step_y = -dx;
    long long offset_x = (map.x_size * (1 - (dx + step_x))) / 2;
    long long offset_y = (map.y_size * (1 - (dy + step_y))) / 2;
    // just fudge it
    offset_x += (offset_x != 0) ? (-1) : 0;
    offset_y += (offset_y != 0) ? (-1) : 0;

    const int endstop = (step_x != 0) ? map.x_size : map.y_size;
    for (int i = 0; i < endstop; i++) {
      D16State modified_initial_state = {offset_x + i * step_x - dx,
                                         offset_y + i * step_y - dy, facing};
      const long long curr_res = solve(&map, modified_initial_state);
      curr_best = max(curr_best, curr_res);
    }
  }
  res.right = curr_best;

  free((map.map));
  return res;
}

AocDayRes solve_year23_day16(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);

  LLTuple res = year23_day16(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
