#include "day17.h"
#include "../../res/gheap.h"
#include "../../res/hashmap.c/hashmap.h"

#define MAX_SIZE 10000
#define HEAP_LIMIT 1000000

#define D17_PART2

#define DISTANCE_UNREACHABLE -1

enum FACING {
  FACING_north = 0,
  FACING_east = 1,
  FACING_south = 2,
  FACING_west = 3
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

static enum FACING turn_facing(enum FACING in, bool left) {
  const int offset = (left) ? (-1) : 1;
  const int facing_val = (int)in;
  return (4 + facing_val + offset) % 4;
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
  int x;
  int y;
  enum FACING facing;
  int straight_moves;
  int distance;
  int heuristic;
} D17State;

static const D17State INVALID_STATE = {
    .x = -2, .y = -2, FACING_west, -1, 1000000, 1000000};

void print_d17state(const D17State *state) {
  printf("state:(%d,%d,%d,%d (cost:%d,latest heuristic: %d))\n", state->x,
         state->y, state->facing, state->straight_moves, state->distance,
         state->heuristic);
}

static int d17state_compare(const void *a, const void *b, void *udata) {
  const D17State *sa = a;
  const D17State *sb = b;
  const int x_cmp = CMP(sa->x, sb->x);
  if (x_cmp != 0)
    return x_cmp;
  const int y_cmp = CMP(sa->y, sb->y);
  if (y_cmp != 0)
    return y_cmp;
  const int straight_compare = CMP(sa->straight_moves, sb->straight_moves);
  if (straight_compare != 0)
    return straight_compare;
  return CMP(sa->facing, sb->facing);
}

static u_int64_t d17state_hash(const void *item, u_int64_t seed0,
                               u_int64_t seed1) {
  const D17State *res = item;
  long long hash_data = MAX_SIZE * MAX_SIZE * (res->x) + MAX_SIZE * (res->y) +
                        8 * (res->straight_moves) + (res->facing);
  uint64_t hash_val = hashmap_sip(&hash_data, (sizeof(hash_data)), 0, 0);
  return hash_val;
}

typedef struct {
  int *map;
  int x_size;
  int y_size;
} D17Map;

static void d17map_set(const D17Map *map, int x, int y, int val) {
  map->map[map->y_size * x + y] = val;
}

static int d17map_lookup(const D17Map *map, int x, int y) {
  if ((0 <= x) && (x < map->x_size) && (0 <= y) && (y < map->y_size))
    return map->map[(map->y_size) * x + y];
  return DISTANCE_UNREACHABLE;
}

// moves state 1 step
static D17State step(D17State state, const D17Map *map) {
  LLTuple delta = facing_to_delta(state.facing);
  state.x += (int)delta.left;
  state.y += (int)delta.right;
  state.straight_moves += 1;
  const int distance = d17map_lookup(map, state.x, state.y);
  if (distance != DISTANCE_UNREACHABLE)
    state.distance += distance;
  else
    state = INVALID_STATE;
  return state;
}

static void get_neighbors(const D17Map *map, D17State state,
                          D17State **outvec) {
  if (state.straight_moves < 3) {
    D17State straight_state = step(state, map);
    cvector_push_back(*outvec, straight_state);
  }
  enum FACING turned_facing[] = {turn_facing(state.facing, false),
                                 turn_facing(state.facing, true)};
  for (int i = 0; i < 2; i++) {
    D17State turned_state = state;
    turned_state.facing = turned_facing[i];
    turned_state.straight_moves = 0;
    turned_state = step(turned_state, map);
    cvector_push_back(*outvec, turned_state);
  }
}

static bool is_final(const D17Map *map, D17State state) {
  return (state.x == map->x_size - 1) && (state.y == map->y_size - 1);
}

static void get_neighbors_naive(const D17Map *map,const D17State state,
                                D17State **outvec) {
  for (int i = 0; i < 4; i++) {
    LLTuple delta = facing_to_delta(i);
    D17State moved_state = state;
    moved_state.facing = 0;
    moved_state.x += delta.left;
    moved_state.y += delta.right;
    moved_state.distance += d17map_lookup(map, state.x, state.y);
    if (0 <= (moved_state.x) && (moved_state.x < (map->x_size)) &&
        (0 <= moved_state.y) && (moved_state.y < (map->y_size))) {
      cvector_push_back(*outvec, moved_state);
    }
  }
}

static bool is_final_heuristic(const D17Map *map, D17State state) {
  return false;
}

static bool is_final_ultra(const D17Map *map, D17State state) {
  return (state.x == map->x_size - 1) && (state.y == map->y_size - 1) &&
         (state.straight_moves >= 4);
}

static void get_neighbors_ultra(const D17Map *map,const D17State state,
                                D17State **outvec) {
  if (state.straight_moves < 10) {
    D17State straight_state = step(state, map);
    cvector_push_back(*outvec, straight_state);
  }
  if (state.straight_moves >= 4) {
    enum FACING turned_facing[] = {turn_facing(state.facing, false),
                                   turn_facing(state.facing, true)};
    for (int i = 0; i < 2; i++) {
      D17State turned_state = state;
      turned_state.facing = turned_facing[i];
      turned_state.straight_moves = 0;
      turned_state = step(turned_state, map);
      cvector_push_back(*outvec, turned_state);
    }
  }
  // printf("generated:%ld nbs.\n", cvector_size(*outvec));
}

static int less(const void *const ctx, const void *const a,
                const void *const b) {
  (void)ctx;
  const D17State *sa = a;
  const D17State *sb = b;
  if (sa->heuristic != sb->heuristic)
    return (sa->heuristic > sb->heuristic);
  return sa->distance > sb->distance;
}

static void move(void *const dst, const void *const src) {
  *(D17State *)dst = *(D17State *)src;
}

static const struct gheap_ctx heap_ctx = {
    .fanout = 2,
    .page_chunks = 1,
    .item_size = sizeof(D17State),
    .less_comparer = &less,
    .less_comparer_ctx = NULL,
    .item_mover = &move,
};

typedef struct {
  struct hashmap *visited;
} SolveResult;

typedef struct {
  int *heuristic_distances;
  int x_size;
  int y_size;
} HeuristicDistances;

int heuristic_use_naive_dist(D17State state, const void *heuristic_data) {
  const HeuristicDistances *res = (HeuristicDistances *)heuristic_data;
  if (0 <= (state.x) && (state.x < res->x_size) && (0 <= state.y) &&
      (state.y < res->y_size)) {
    return res->heuristic_distances[state.x * res->y_size + state.y];
  }
  return 0;
}

void check_distances_are_consistent(const HeuristicDistances* distances){

}


typedef void (*get_nbs_ptr)(const D17Map *map, D17State state,
                            D17State **outvec);

typedef bool (*is_final_ptr)(const D17Map *map, D17State state);

typedef int (*heuristic_ptr)(D17State state, const void *heuristic_data);

static long long solve(const D17Map *map, D17State *initial_state_vec,
                       get_nbs_ptr get_nbs, is_final_ptr is_final_,
                       heuristic_ptr heuristic, void *heuristic_data,
                       SolveResult *out_solve) {
  struct hashmap *states = hashmap_new(sizeof(D17State), 0, 0, 0, d17state_hash,
                                       d17state_compare, NULL, NULL);

  struct hashmap *visited = hashmap_new(
      sizeof(D17State), 0, 0, 0, d17state_hash, d17state_compare, NULL, NULL);

  out_solve->visited = visited;

  D17State *nb_vec = NULL;

  // start right off the map

  D17State *const priority_heap = malloc(sizeof(D17State) * HEAP_LIMIT);
  size_t curr_heap_size = 0;

  gheap_make_heap(&heap_ctx, priority_heap, curr_heap_size);

  for (int i = 0; i < cvector_size(initial_state_vec); i++) {
    const D17State initial_state = initial_state_vec[i];
    hashmap_set(states, &initial_state);
    priority_heap[curr_heap_size++] = initial_state;
    gheap_push_heap(&heap_ctx, priority_heap, curr_heap_size);
  }

  int steps = 0;
  while (curr_heap_size > 0) {
    steps++;
    // assert(steps < 50000);
    gheap_pop_heap(&heap_ctx, priority_heap, curr_heap_size);
    const D17State curr_state = priority_heap[curr_heap_size - 1];
    curr_heap_size--;
    bool already_visited = hashmap_get(visited, &curr_state) != NULL;
    if (already_visited) {
      continue;
    }
    bool is_final = is_final_(map, curr_state);
    if (is_final) {
      return curr_state.distance;
    }
    hashmap_set(visited, &curr_state);
    cvector_clear(nb_vec);
    get_nbs(map, curr_state, &nb_vec);
    for (int i = 0; i < cvector_size(nb_vec); i++) {
      D17State *nb = &(nb_vec[i]);
      int heuristic_dist = 0;
      if (heuristic != NULL) {
        heuristic_dist = heuristic(*nb, heuristic_data);
        const int distance_to_nb = d17map_lookup(map, nb->x, nb->y);
        nb->heuristic = curr_state.distance + distance_to_nb+heuristic_dist;
        //printf("setting heurdist to %d+%d+%d=%d\n",curr_state.distance,distance_to_nb,heuristic_dist,nb->heuristic);
      }
      D17State *old_nb_data=hashmap_get(visited,nb);
      if(old_nb_data!=NULL){
        assert(old_nb_data->heuristic<=nb->heuristic);
      }
      priority_heap[curr_heap_size++] = *nb;
      gheap_push_heap(&heap_ctx, priority_heap, curr_heap_size);
    }
  }
  return 0;
}

static void fill_map(D17Map *map, char *buf, long buf_len) {
  const int y_size = strchr(buf, '\n') - buf;
  const int x_size = buf_len / (y_size + 1);
  map->x_size = x_size;
  map->y_size = y_size;
  map->map = malloc(sizeof(*map->map) * map->x_size * map->y_size);
  int curr_x = 0;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    for (int i = 0; i < map->y_size; i++) {
      d17map_set(map, curr_x, i, line[i] - '0');
    }
    curr_x++;
  }
}

LLTuple year23_day17(char *buf, long buf_len) {
  LLTuple res = {0};
  D17Map map = {0};
  fill_map(&map, buf, buf_len);
  D17State *initial_states = NULL;
  enum FACING initial_facings[] = {FACING_east, FACING_south};
  for (int i = 0; i < 2; i++) {
    const enum FACING initial_facing = initial_facings[i];
    D17State initial_state = {.x = 0,
                              .y = 0,
                              .facing = initial_facing,
                              .straight_moves = 0,
                              .distance = 0,
                              .heuristic = 0};
    cvector_push_back(initial_states, initial_state);
  }

  D17State *heuristic_initial_states = NULL;
  D17State heuristic_initial_state = {.x = map.x_size - 1,
                                      .y = map.y_size - 1,
                                      .facing = 0,
                                      .straight_moves = 0,
                                      .distance = 0,
                                      .heuristic = 0};
  cvector_push_back(heuristic_initial_states, heuristic_initial_state);

  SolveResult heuristic_result = {NULL};

  solve(&map, heuristic_initial_states, get_neighbors_naive, is_final_heuristic,
        NULL, NULL, &heuristic_result);

  HeuristicDistances heuristic_distances = {.x_size = map.x_size,
                                            .y_size = map.y_size};
  heuristic_distances.heuristic_distances =
      malloc(sizeof((*heuristic_distances.heuristic_distances)) * (map.x_size) *
             (map.y_size));

  size_t iter = 0;
  void *item;
  while (hashmap_iter(heuristic_result.visited, &iter, &item)) {
    const D17State *state = item;
    heuristic_distances
        .heuristic_distances[(state->x) * heuristic_distances.y_size +
                             state->y] = state->distance;
  }
  // return res;
  SolveResult result = {NULL};
  res.left = solve(&map, initial_states, get_neighbors, is_final,
                   heuristic_use_naive_dist, &heuristic_distances, &result);

  res.right = solve(&map, initial_states, get_neighbors_ultra, is_final_ultra,
                    heuristic_use_naive_dist, &heuristic_distances, &result);

  //  res.left = solve(&map, initial_states, get_neighbors, is_final,NULL,NULL,
  //  &result);
  //
  //  res.right = solve(&map, initial_states, get_neighbors_ultra,
  //  is_final_ultra,NULL,NULL,&result);

  free((map.map));
  return res;
}

AocDayRes solve_year23_day17(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);

  LLTuple res = year23_day17(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
