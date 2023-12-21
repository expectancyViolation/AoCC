#include "day21.h"
#include "../../res/hashmap.c/hashmap.h"
#include <stdint-gcc.h>

#define MAX_SIZE 100000

#define DISTANCE_UNREACHABLE (-1)

#define MOD_(a, M) (((a) % (M)) + (M)) % (M)

typedef struct {
  char *map;
  int x_size;
  int y_size;
  int start_x;
  int start_y;
} D21Map;

typedef struct {
  int x;
  int y;
  int distance;
} D21State;

static const LLTuple DELTAS[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
#define NUM_DELTAS 4

static int d21state_compare(const void *a, const void *b, void *udata) {
  const D21State *sa = a;
  const D21State *sb = b;
  const int x_cmp = CMP(sa->x, sb->x);
  if (x_cmp != 0)
    return x_cmp;
  const int y_cmp = CMP(sa->y, sb->y);
  return y_cmp;
}

static void d21map_set(const D21Map *map, int x, int y, int val) {
  map->map[map->y_size * x + y] = val;
}

static char d21_map_periodic_lookup(const D21Map *map, int x, int y) {
  const int x_offset = x + (map->start_x);
  const int y_offset = y + (map->start_y);
  const int x_rem = MOD_(x_offset, map->x_size);
  const int y_rem = MOD_(y_offset, map->y_size);

  return map->map[(map->y_size) * x_rem + y_rem];
}

static u_int64_t d21state_hash(const void *item, u_int64_t seed0,
                               u_int64_t seed1) {
  const D21State *res = item;
  long long hash_data = MAX_SIZE * (res->x) + (res->y);
  uint64_t hash_val = hashmap_sip(&hash_data, (sizeof(hash_data)), 0, 0);
  return hash_val;
}

static void fill_map(D21Map *map, char *buf, long buf_len) {
  const int y_size = strchr(buf, '\n') - buf;
  const int x_size = buf_len / (y_size + 1);
  map->x_size = x_size;
  map->y_size = y_size;
  map->map = malloc(sizeof(*map->map) * x_size * y_size);
  int curr_x = 0;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    for (int i = 0; i < y_size; i++) {
      d21map_set(map, curr_x, i, line[i]);
      if (line[i] == 'S') {
        map->start_x = curr_x;
        map->start_y = i;
      }
    }
    curr_x++;
  }
}

static LLTuple solve(const D21Map *map) {

  struct hashmap *visited = hashmap_new(
      sizeof(D21State), 0, 0, 0, d21state_hash, d21state_compare, NULL, NULL);

  struct hashmap *frontier = hashmap_new(
      sizeof(D21State), 0, 0, 0, d21state_hash, d21state_compare, NULL, NULL);

  hashmap_set(frontier, &(D21State){.x = 0, .y = 0, .distance = 0});

  struct hashmap *new_frontier = hashmap_new(
      sizeof(D21State), 0, 0, 0, d21state_hash, d21state_compare, NULL, NULL);

  for (int n_step = 0; n_step < 4 * map->x_size; n_step++) {
    // limit should be safe to reach all reachable positions in 3x3 blocks

    size_t iter = 0;
    void *item;
    while (hashmap_iter(frontier, &iter, &item)) {
      D21State curr_state = *(D21State *)item;

      hashmap_set(visited, &curr_state);
      for (int i = 0; i < NUM_DELTAS; i++) {
        const int nx = curr_state.x + DELTAS[i].left;
        const int ny = curr_state.y + DELTAS[i].right;
        if (d21_map_periodic_lookup(map, nx, ny) != '#') {
          if (hashmap_get(visited, &(D21State){.x = nx, .y = ny}) != NULL)
            continue;
          const D21State new_state = {
              .x = nx, .y = ny, .distance = curr_state.distance + 1};
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

  const long long step_count = 26501365;

  size_t iter = 0;
  void *item;
  long long count_at_odd_dist = 0;
  long long count_even_below_64 = 0;
  long long block_size = map->x_size;
  while (hashmap_iter(visited, &iter, &item)) {
    D21State curr_state = *(D21State *)item;

    int pos_inside_block_x = MOD_(curr_state.x + map->start_x, map->x_size);
    int pos_inside_block_y = MOD_(curr_state.y + map->start_y, map->y_size);
    int block_x =
        (curr_state.x + map->start_x - pos_inside_block_x) / (map->x_size);
    int block_y =
        (curr_state.y + map->start_y - pos_inside_block_y) / (map->y_size);

    if ((abs(block_x) > 1) || (abs(block_y) > 1))
      continue;

    long long remaining_dist = step_count - curr_state.distance;
    long long remaining_block_steps = remaining_dist / block_size;

    bool is_odd_distance = (curr_state.distance % 2) == 1;

    if (curr_state.distance <= 64) {
      count_even_below_64 += (!is_odd_distance);
    }

    if ((block_x == 0) && (block_y == 0)) {
      // initial block
      count_at_odd_dist += is_odd_distance;
    } else if (abs(block_x) + abs(block_y) == 1) {
      // straight line alternating odd even
      count_at_odd_dist += (remaining_block_steps + 1 + is_odd_distance) / 2;
    } else {
      // striped alternating triangles in quadrant
      if (is_odd_distance) {
        long long n_ = (remaining_block_steps + 2) / 2;
        count_at_odd_dist += n_ * n_;
      } else {
        long long n_ = (remaining_block_steps + 1) / 2;
        count_at_odd_dist += n_ * (n_ + 1);
      }
    }
  }

  return (LLTuple){count_even_below_64, count_at_odd_dist};
}

LLTuple year23_day21(char *buf, long buf_len) {
  LLTuple res = {0};
  D21Map map = {0};
  fill_map(&map, buf, buf_len);
  res = solve(&map);
  return res;
}
AocDayRes solve_year23_day21(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day21(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
