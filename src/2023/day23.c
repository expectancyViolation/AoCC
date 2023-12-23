#include "day23.h"
#include "../../res/hashmap.c/hashmap.h"
#include <stdint-gcc.h>

#define MAX_SIZE 10000
#define MOD_(a, M) (((a) % (M)) + (M)) % (M)

enum FACING {
  FACING_north = 0,
  FACING_east = 1,
  FACING_south = 2,
  FACING_west = 3
};

#define NUM_FACING 4

typedef struct {
  char *map;
  int x_size;
  int y_size;
  int start_x;
  int start_y;
} D23Map;

typedef struct {
  int x;
  int y;
  enum FACING facing;
  int distance;
} D23State;

void print_d23state(D23State const *state) {
  printf("STATE: %d,%d,f:%d,dist:%d\n", state->x, state->y, state->facing,
         state->distance);
}

static const LLTuple DELTAS[] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
#define NUM_DELTAS 4

bool d23_tile_move_allowed(char tile, int delta_id) {

  switch (tile) {
  case '.':
    return true;
  case '>':
    return delta_id == 1;
  case 'v':
    return delta_id == 2;
  default:
    assert(false);
  }
}

bool d23_facing_move_allowed(enum FACING facing, int delta_id) {

  switch (facing) {
  case FACING_north:
    return delta_id != 2;
  case FACING_east:
    return delta_id != 3;
  case FACING_south:
    return delta_id != 0;
  case FACING_west:
    return delta_id != 1;
  default:
    assert(false);
  }
}

static int d23state_compare(const void *a, const void *b, void *udata) {
  const D23State *sa = a;
  const D23State *sb = b;
  const int x_cmp = CMP(sa->x, sb->x);
  if (x_cmp != 0)
    return x_cmp;
  const int y_cmp = CMP(sa->y, sb->y);
  if (y_cmp != 0)
    return y_cmp;
  return CMP(sa->facing, sb->facing);
}

static void d23map_set(const D23Map *map, int x, int y, int val) {
  map->map[map->y_size * x + y] = val;
}

static char d23_map_lookup(const D23Map *map, int x, int y) {
  if ((0 <= x) && (x < map->x_size) && (0 <= y) && (y < map->y_size))
    return map->map[(map->y_size) * x + y];
  return '#';
}

static u_int64_t d23state_hash(const void *item, u_int64_t seed0,
                               u_int64_t seed1) {
  const D23State *res = item;
  long long hash_data =
      NUM_FACING * (MAX_SIZE * (res->x) + (res->y)) + res->facing;
  uint64_t hash_val = hashmap_sip(&hash_data, (sizeof(hash_data)), 0, 0);
  return hash_val;
}

static void fill_map(D23Map *map, char *buf, long buf_len) {
  const int y_size = strchr(buf, '\n') - buf;
  const int x_size = buf_len / (y_size + 1);
  map->x_size = x_size;
  map->y_size = y_size;
  map->map = malloc(sizeof(*map->map) * x_size * y_size);
  int curr_x = 0;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    for (int i = 0; i < y_size; i++) {
      d23map_set(map, curr_x, i, line[i]);
      if ((curr_x == 0) && (line[i] == '.')) {
        map->start_x = curr_x;
        map->start_y = i;
      }
    }
    curr_x++;
  }
}

static LLTuple solve(const D23Map *map) {

  struct hashmap *visited = hashmap_new(
      sizeof(D23State), 0, 0, 0, d23state_hash, d23state_compare, NULL, NULL);

  struct hashmap *frontier = hashmap_new(
      sizeof(D23State), 0, 0, 0, d23state_hash, d23state_compare, NULL, NULL);

  hashmap_set(frontier, &(D23State){.x = map->start_x,
                                    .y = map->start_y,
                                    .facing = FACING_south,
                                    .distance = 0});

  struct hashmap *new_frontier = hashmap_new(
      sizeof(D23State), 0, 0, 0, d23state_hash, d23state_compare, NULL, NULL);

  while(hashmap_count(frontier)>0) {
    // limit should be safe to reach all reachable positions in 3x3 blocks

    size_t iter = 0;
    void *item;
    while (hashmap_iter(frontier, &iter, &item)) {
      D23State curr_state = *(D23State *)item;

      hashmap_set(visited, &curr_state);
      //printf("visiting: ");
      //print_d23state(&curr_state);
      char curr_tile = d23_map_lookup(map, curr_state.x, curr_state.y);
      for (int i = 0; i < NUM_DELTAS; i++) {
        //printf("checking: %d\n", i);
        if (!d23_tile_move_allowed(curr_tile, i))
          continue;
        if (!d23_facing_move_allowed(curr_state.facing, i))
          continue;

        const int nx = curr_state.x + DELTAS[i].left;
        const int ny = curr_state.y + DELTAS[i].right;
        const int nfacing = i;
        if (d23_map_lookup(map, nx, ny) != '#') {
          //printf("oki\n");
          int old_best_distance = 0;
          D23State const *old_visit = hashmap_get(
              visited, &(D23State){.x = nx, .y = ny, .facing = nfacing});
          if (old_visit != NULL) {
            old_best_distance = old_visit->distance;
          }
          const D23State new_state = {
              .x = nx,
              .y = ny,
              .facing = nfacing,
              .distance = max(old_best_distance, curr_state.distance + 1)};
          hashmap_set(visited, &new_state);
          hashmap_set(new_frontier, &new_state);
        }
      }
    }
    struct hashmap *tmp = frontier;
    frontier = new_frontier;
    new_frontier = tmp;
    hashmap_clear(new_frontier, 0);
  }

  size_t iter = 0;
  void *item;
  long long count_at_odd_dist = 0;
  long long count_even_below_64 = 0;
  long long block_size = map->x_size;
  while (hashmap_iter(visited, &iter, &item)) {
    D23State *curr_state = (D23State *)item;
    if(curr_state->x==map->x_size-1) {
      print_d23state(curr_state);
    }
  }

  return (LLTuple){count_even_below_64, count_at_odd_dist};
}

LLTuple year23_day23(char *buf, long buf_len) {
  LLTuple res = {0};
  D23Map map = {0};
  fill_map(&map, buf, buf_len);
  solve(&map);
  return res;
}
AocDayRes solve_year23_day23(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day23(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
