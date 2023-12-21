#include "day21.h"
#include "../../res/hashmap.c/hashmap.h"
#include <stdint-gcc.h>

#define MAX_SIZE 100000

#define DISTANCE_UNREACHABLE (-1)

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

static char d21map_lookup(const D21Map *map, int x, int y) {
  if ((0 <= x) && (x < map->x_size) && (0 <= y) && (y < map->y_size))
    return map->map[(map->y_size) * x + y];
  return '#';
}

static u_int64_t d21state_hash(const void *item, u_int64_t seed0,
                               u_int64_t seed1) {
  const D21State *res = item;
  long long hash_data = MAX_SIZE * (res->x) + (res->y);
  uint64_t hash_val = hashmap_sip(&hash_data, (sizeof(hash_data)), 0, 0);
  return hash_val;
}

typedef struct {
  struct hashmap *visited;
} SolveResult;

static void fill_map(D21Map *map, char *buf, long buf_len) {
  const int y_size = strchr(buf, '\n') - buf;
  const int x_size = buf_len / (y_size + 1);
  map->x_size = 3 * x_size;
  map->y_size = 3 * y_size;
  map->map = malloc(9 * sizeof(*map->map) * x_size * y_size);
  int curr_x = 0;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    for (int i = 0; i < y_size; i++) {
      for (int j = 0; j < 3; j++) {
        for (int k = 0; k < 3; k++) {
          char symbol = ((j == 1) || (k == 1)) ? line[i] : '#';
          d21map_set(map, curr_x, i, line[i]);
          if ((k == 1) && (j == 1) && (line[i] == 'S')) {
            map->start_x = x_size + curr_x;
            map->start_y = y_size + i;
          }
        }
      }
    }
    curr_x++;
  }
}

static long long solve(const D21Map *map) {

  struct hashmap *visited = hashmap_new(
      sizeof(D21State), 0, 0, 0, d21state_hash, d21state_compare, NULL, NULL);

  struct hashmap *frontier = hashmap_new(
      sizeof(D21State), 0, 0, 0, d21state_hash, d21state_compare, NULL, NULL);

  hashmap_set(frontier,
              &(D21State){.x = map->start_x, .y = map->start_y, .distance = 0});

  struct hashmap *new_frontier = hashmap_new(
      sizeof(D21State), 0, 0, 0, d21state_hash, d21state_compare, NULL, NULL);

  // int steps = 0;
  for (int n_step = 0; n_step < 800; n_step++) {

    size_t iter = 0;
    void *item;
    while (hashmap_iter(frontier, &iter, &item)) {
      D21State curr_state = *(D21State *)item;
      // printf("checking: %d %d\n", curr_state.x, curr_state.y);
      //     bool already_visited = hashmap_get(visited, &curr_state) != NULL;
      //     if (already_visited) {
      //       assert(false);
      //       continue;
      //     }
      // printf("visiting:%d %d at %d\n", curr_state.x,
      // curr_state.y,curr_state.distance);
      hashmap_set(visited, &curr_state);
      for (int i = 0; i < NUM_DELTAS; i++) {
        const int nx = curr_state.x + DELTAS[i].left;
        const int ny = curr_state.y + DELTAS[i].right;
        if (d21map_lookup(map, nx, ny) != '#') {
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
    printf("frontier: %ld\n", hashmap_count(frontier));
    printf("visited: %ld\n", hashmap_count(visited));
  }

  size_t iter = 0;
  void *item;
  long long count_at_even_dist = 0;
  long long count_p1 = 0;
  long long block_size = map->x_size / 3;
  printf("block size is %lld\n", block_size);
  int count_at_even_dist_p2 = 0;
  while (hashmap_iter(visited, &iter, &item)) {
    D21State curr_state = *(D21State *)item;
    bool is_even_dist = (curr_state.distance % 2) == 0;
    if (!is_even_dist)
      continue;
    //    if (curr_state.distance == 0)
    //      curr_state.distance = 2;

    // handle regular distances (p1)
    if ((abs(curr_state.x - (map->start_x)) <= 65) &&
        abs(curr_state.y - (map->start_y)) <= 65) {
      count_p1 += is_even_dist;
      continue;
    }

    int remaining_dist = 26501365 - curr_state.distance;
    // printf("%d\n", map->x_size);
    long long remaining_block_steps = remaining_dist / block_size;
    //  1199438254480527 too high
    //  1200175023393127
    //  1404644746729180
    //    85740177832380
    //   771646337293980 wrong
    //  1369576921329408
    count_at_even_dist +=
        ((remaining_block_steps + 1) * (remaining_block_steps + 1)) / 2;
  }
  printf("%lld\n", count_p1);
  return count_at_even_dist;
}

LLTuple year23_day21(char *buf, long buf_len) {
  LLTuple res = {0};
  D21Map map = {0};
  fill_map(&map, buf, buf_len);
  res.left = solve(&map);
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
