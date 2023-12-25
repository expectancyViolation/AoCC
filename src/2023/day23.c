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
  int x;
  int y;
  enum FACING facing;
  int previous_crossing_index;
  int crossing_index;
  int distance_to_previous_crossing;
} D23State;

typedef struct {
  char *map;
  int x_size;
  int y_size;
  int start_x;
  int start_y;
  D23State *crossings_vec;
  LLTuple *crossing_connections_vec;
  int *crossing_connection_weights_vec;
} D23Map;

void print_d23state(D23State const *state) {
  printf("STATE: %d,%d,f:%d,crossing:%d,prev crossing:%d\n", state->x, state->y,
         state->facing, state->crossing_index, state->previous_crossing_index);
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
  return y_cmp;
}

static u_int64_t d23state_hash(const void *item, u_int64_t seed0,
                               u_int64_t seed1) {
  const D23State *res = item;
  long long hash_data = NUM_FACING * (MAX_SIZE * (res->x) + (res->y));
  uint64_t hash_val = hashmap_sip(&hash_data, (sizeof(hash_data)), 0, 0);
  return hash_val;
}

static void d23map_set(const D23Map *map, int x, int y, int val) {
  map->map[map->y_size * x + y] = val;
}

static char d23_map_lookup(const D23Map *map, int x, int y) {
  if ((0 <= x) && (x < map->x_size) && (0 <= y) && (y < map->y_size))
    return map->map[(map->y_size) * x + y];
  return '#';
}

static bool d23_is_crossing(const D23Map *map, int x, int y) {
  if ((x == 0) || (x == map->x_size - 1))
    return true;
  int nbs = 0;
  for (int i = 0; i < NUM_DELTAS; i++) {
    const int nx = x + DELTAS[i].left;
    const int ny = y + DELTAS[i].right;
    nbs += '#' != (d23_map_lookup(map, nx, ny));
  }
  return nbs >= 3;
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

typedef struct {
  uint64_t visited_crossing_bitset;
  int curr_crossing;
} D23LongTripPosition;

typedef struct {
  D23LongTripPosition position;
  int distance;
} D23LongTripState;

void print_d23_long_trip_state(D23LongTripState const *state) {
  printf("LTS: %lu %d dist:%d\n", state->position.visited_crossing_bitset,
         state->position.curr_crossing, state->distance);
}

static int d23long_trip_state_compare(const void *a, const void *b,
                                      void *udata) {
  const D23LongTripState *sa = a;
  const D23LongTripState *sb = b;
  const int vis_cmp = CMP(sa->position.visited_crossing_bitset,
                          sb->position.visited_crossing_bitset);
  if (vis_cmp != 0)
    return vis_cmp;
  return CMP(sa->position.curr_crossing, sb->position.curr_crossing);
}

static u_int64_t d23long_trip_state_hash(const void *item, u_int64_t seed0,
                                         u_int64_t seed1) {
  const D23LongTripState *res = item;
  uint64_t hash_val =
      hashmap_sip(&(res->position), (sizeof(D23LongTripPosition)), 0, 0);
  return hash_val;
}

static inline uint64_t
kbi_popcount64(uint64_t y) // standard popcount; from wikipedia
{
  y -= ((y >> 1) & 0x5555555555555555ull);
  y = (y & 0x3333333333333333ull) + (y >> 2 & 0x3333333333333333ull);
  return ((y + (y >> 4)) & 0xf0f0f0f0f0f0f0full) * 0x101010101010101ull >> 56;
}

static LLTuple solve(D23Map *map) {

  struct hashmap *visited_crossings = hashmap_new(
      sizeof(D23State), 0, 0, 0, d23state_hash, d23state_compare, NULL, NULL);

  struct hashmap *frontier = hashmap_new(
      sizeof(D23State), 0, 0, 0, d23state_hash, d23state_compare, NULL, NULL);

  hashmap_set(frontier, &(D23State){.x = map->start_x,
                                    .y = map->start_y,
                                    .facing = FACING_south,
                                    .crossing_index = -1,
                                    .previous_crossing_index = -1,
                                    .distance_to_previous_crossing = 0});

  struct hashmap *new_frontier = hashmap_new(
      sizeof(D23State), 0, 0, 0, d23state_hash, d23state_compare, NULL, NULL);

  while (hashmap_count(frontier) > 0) {
    // limit should be safe to reach all reachable positions in 3x3 blocks

    size_t iter = 0;
    void *item;
    while (hashmap_iter(frontier, &iter, &item)) {
      D23State curr_state = *(D23State *)item;
      bool already_visited =
          hashmap_get(visited_crossings, &curr_state) != NULL;

      bool curr_is_crossing = d23_is_crossing(map, curr_state.x, curr_state.y);
      if (curr_is_crossing) {
        D23State *prev_visit_state =
            hashmap_get(visited_crossings, &curr_state);
        if (prev_visit_state == NULL) {
          int crossing_index = cvector_size(map->crossings_vec);
          curr_state.crossing_index = crossing_index;
          cvector_push_back(map->crossings_vec, curr_state);
        } else {
          curr_state.crossing_index = prev_visit_state->crossing_index;
        }
        LLTuple edge = {.left = curr_state.previous_crossing_index,
                        .right = curr_state.crossing_index};
        cvector_push_back(map->crossing_connections_vec, edge);
        cvector_push_back(map->crossing_connection_weights_vec,
                          curr_state.distance_to_previous_crossing);
        hashmap_set(visited_crossings, &curr_state);
        curr_state.previous_crossing_index = curr_state.crossing_index;
        curr_state.distance_to_previous_crossing = 0;
      }
      if (already_visited)
        continue;
      //      printf("visiting: ");
      //      print_d23state(&curr_state);
      //      printf("is crossing:%d\n", curr_is_crossing);
      char curr_tile = d23_map_lookup(map, curr_state.x, curr_state.y);
      for (int i = 0; i < NUM_DELTAS; i++) {
        //        printf("checking: %d\n", i);
        //        if (!d23_tile_move_allowed(curr_tile, i))
        //          continue;
        if ((!d23_facing_move_allowed(curr_state.facing, i)))
          continue;

        const int nx = curr_state.x + DELTAS[i].left;
        const int ny = curr_state.y + DELTAS[i].right;
        const int nfacing = i;
        if (d23_map_lookup(map, nx, ny) != '#') {
          // printf("oki\n");
          const D23State new_state = {
              .x = nx,
              .y = ny,
              .facing = nfacing,
              .previous_crossing_index = curr_state.previous_crossing_index,
              .crossing_index = -1,
              .distance_to_previous_crossing =
                  curr_state.distance_to_previous_crossing + 1};
          // hashmap_set(visited_crossings, &new_state);
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
  long long block_size = map->x_size;
  while (hashmap_iter(visited_crossings, &iter, &item)) {
    //printf("crossing:\n");
    D23State *curr_state = (D23State *)item;
    //print_d23state(curr_state);
  }
  LLTuple **neighbors =
      malloc(cvector_size(map->crossings_vec) * sizeof(LLTuple *));
  for (int i = 0; i < cvector_size(map->crossings_vec); i++)
    neighbors[i] = NULL;

  for (int i = 0; i < cvector_size(map->crossing_connections_vec); i++) {
    const LLTuple edge = map->crossing_connections_vec[i];
    if (edge.left == -1)
      continue;
    //    printf("%lld,%lld,Undirected,%d,%d,1\n", edge.left, edge.right, i,
    //           map->crossing_connection_weights_vec[i]);
    LLTuple left_entry = (LLTuple){
        .left = edge.right, .right = map->crossing_connection_weights_vec[i]};
    cvector_push_back(neighbors[edge.left], left_entry);

    LLTuple right_entry = (LLTuple){
        .left = edge.left, .right = map->crossing_connection_weights_vec[i]};
    cvector_push_back(neighbors[edge.right], right_entry);
  }

//  for (int i = 0; i < cvector_size(map->crossings_vec); i++) {
//    //print_d23state(&map->crossings_vec[i]);
//    for (int j = 0; j < cvector_size(neighbors[i]); j++) {
//      print_tuple(neighbors[i][j]);
//      printf("\n");
//    }
//  }

  struct hashmap *long_trip_visited =
      hashmap_new(sizeof(D23LongTripState), 0, 0, 0, d23long_trip_state_hash,
                  d23long_trip_state_compare, NULL, NULL);

  struct hashmap *long_trip_frontier =
      hashmap_new(sizeof(D23LongTripState), 0, 0, 0, d23long_trip_state_hash,
                  d23long_trip_state_compare, NULL, NULL);

  struct hashmap *long_trip_new_frontier =
      hashmap_new(sizeof(D23LongTripState), 0, 0, 0, d23long_trip_state_hash,
                  d23long_trip_state_compare, NULL, NULL);

  hashmap_set(long_trip_frontier,
              &(D23LongTripState){.position = {.curr_crossing = 0,
                                               .visited_crossing_bitset = 1},
                                  .distance = 0});

  while (hashmap_count(long_trip_frontier) > 0) {
    // limit should be safe to reach all reachable positions in 3x3 blocks

    size_t iter = 0;
    void *item;
    while (hashmap_iter(long_trip_frontier, &iter, &item)) {
      D23LongTripState curr_state = *(D23LongTripState *)item;

      //      if (curr_state.position.curr_crossing == 2) {
      // print_d23_long_trip_state(&curr_state);
      //      }

      D23LongTripState *prev_visit =
          hashmap_get(long_trip_visited, &curr_state);

      bool distance_updated = true;
      bool already_visited = prev_visit != NULL;
      if (already_visited) {
        distance_updated = curr_state.distance != prev_visit->distance;
        curr_state.distance = max(curr_state.distance, prev_visit->distance);
      }
      hashmap_set(long_trip_visited, &curr_state);
      hashmap_set(new_frontier, &curr_state);
      if (!distance_updated)
        continue;
      int const curr_crossing = curr_state.position.curr_crossing;
      uint64_t const visited_bitset =
          curr_state.position.visited_crossing_bitset;
      LLTuple *nbs = neighbors[curr_crossing];
      for (int i = 0; i < cvector_size(nbs); i++) {
        uint64_t nb = nbs[i].left;
        // printf("%lu\n", nb);
        const uint64_t one = 1;
        uint64_t nb_bit = one << (nb); // DANGER!! literals are 32 bit
        assert(kbi_popcount64(visited_bitset | nb_bit) < 37);
        int nb_dist = nbs[i].right;
        bool nb_already_visited = visited_bitset & nb_bit;
        if (nb_already_visited)
          continue;

        D23LongTripState new_state = {
            .position = {.visited_crossing_bitset = visited_bitset | nb_bit,
                         .curr_crossing = nb},
            .distance = curr_state.distance + nb_dist};

        D23LongTripState *already_in_frontier =
            hashmap_get(long_trip_new_frontier, &new_state);
        if (already_in_frontier != NULL) {
          new_state.distance =
              max(new_state.distance, already_in_frontier->distance);
        }
        hashmap_set(long_trip_new_frontier, &new_state);
      }
    }
    struct hashmap *tmp = long_trip_frontier;
    long_trip_frontier = long_trip_new_frontier;
    long_trip_new_frontier = tmp;
    hashmap_clear(long_trip_new_frontier, 0);
    //printf("frontier size:%ld\n", hashmap_count(long_trip_frontier));
  }

  {
    size_t iter = 0;
    void *item;
    const int end_crossing =
        (cvector_size(map->crossings_vec) - 1); // TODO why?
    int longest_trip = 0;
    while (hashmap_iter(long_trip_visited, &iter, &item)) {
      D23LongTripState curr_state = *(D23LongTripState *)item;
      const uint64_t one = 1;
      if (curr_state.position.curr_crossing == end_crossing)
        longest_trip = max(longest_trip, curr_state.distance);
    }
    return (LLTuple){0,longest_trip};
  }

}

LLTuple year23_day23(char *buf, long buf_len) {
  LLTuple res = {0};
  D23Map map = {0, .crossings_vec = NULL,
                .crossing_connection_weights_vec = NULL};
  fill_map(&map, buf, buf_len);
  return solve(&map);
}
AocDayRes solve_year23_day23(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day23(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
