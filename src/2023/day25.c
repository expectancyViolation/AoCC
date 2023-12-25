#include "day25.h"
#include "../../res/hashmap.c/hashmap.h"

typedef struct {
  u_int64_t hash;
  int offset;
} LabelledOffset;

// hash collisions cause this to fail
static int labelled_offset_compare(const void *a, const void *b, void *udata) {
  const LabelledOffset *sa = a;
  const LabelledOffset *sb = b;
  return CMP(sa->hash, sb->hash);
}

static u_int64_t labelled_offset_hash(const void *item, u_int64_t seed0,
                                      u_int64_t seed1) {
  const LabelledOffset *sa = item;
  return sa->hash;
}

typedef struct {
  struct hashmap *offsets;
  int curr_offset;
} LabelledOffsetGenerator;

LabelledOffsetGenerator generator_init() {
  LabelledOffsetGenerator res = {
      .offsets =
          hashmap_new(sizeof(LabelledOffset), 0, 0, 0, labelled_offset_hash,
                      labelled_offset_compare, NULL, NULL),
      .curr_offset = 0};
  return res;
}

static int get_or_create_offset(LabelledOffsetGenerator *lookup,
                                u_int64_t hash) {
  LabelledOffset const *labelled_offset = hashmap_get(lookup->offsets, &hash);
  if (labelled_offset == NULL) {
    int offset = lookup->curr_offset;
    lookup->curr_offset++;
    hashmap_set(lookup->offsets,
                &(LabelledOffset){.hash = hash, .offset = offset});
    return offset;
  }
  return labelled_offset->offset;
}

u_int64_t hash_string(char *string) {
  return hashmap_sip(string, strlen(string), 0, 0);
}

typedef struct {
  int *others_vec;
  int *offset_vec;
  int *reverse_edge_vec;
} NumberedEdgeLookup;

NumberedEdgeLookup create_edge_lookup(LLTuple *edge_vec) {
  NumberedEdgeLookup res = {
      .offset_vec = NULL, .others_vec = NULL, .reverse_edge_vec = NULL};

  qsort(edge_vec, cvector_size(edge_vec), sizeof(LLTuple),
        (__compar_fn_t)ll_tuple_compare);
  int curr_el = -1;

  for (int i = 0; i < cvector_size(edge_vec); i++) {
    if ((edge_vec[i].left) != curr_el) {
      cvector_push_back(res.offset_vec, cvector_size(res.others_vec));
      curr_el = (edge_vec[i].left);
    }
    cvector_push_back(res.others_vec, edge_vec[i].right);
  }
  cvector_push_back(res.offset_vec, cvector_size(res.others_vec));

  for (int vert1 = 0; vert1 < cvector_size(res.offset_vec) - 1; vert1++) {
    for (int j = res.offset_vec[vert1]; j < res.offset_vec[vert1 + 1]; j++) {
      int vert2 = res.others_vec[j];
      bool found = false;
      for (int lu = res.offset_vec[vert2]; lu < res.offset_vec[vert2 + 1];
           lu++) {
        if (res.others_vec[lu] == vert1) {
          cvector_push_back(res.reverse_edge_vec, lu);
          found = true;
        }
      }
      assert(found);
    }
  }

  return res;
}

typedef struct {
  bool *edge_flow;
} DFSFlow;

LLTuple augment_flow(NumberedEdgeLookup *const lookup, DFSFlow *dfs_flow,
                     int source, int sink) {
  const int num_verts = cvector_size(lookup->offset_vec) - 1;
  bool *visited = malloc(sizeof(bool) * num_verts);
  int *edge_offsets = malloc(sizeof(int) * num_verts);
  LLTuple *pos_stack_vec = NULL;

  for (int i = 0; i < num_verts; i++) {
    visited[i] = false;
  }
  LLTuple initial_state = {.left = source, .right = -1};
  cvector_push_back(pos_stack_vec, initial_state);

  for (int i = 0; i < num_verts; i++) {
    edge_offsets[i] = 0;
  }

  bool found = false;
  while (cvector_size(pos_stack_vec) > 0) {
    int curr_depth = cvector_size(pos_stack_vec) - 1;
    int curr_vert = pos_stack_vec[curr_depth].left;
    int curr_edge_index = edge_offsets[curr_vert];
    pos_stack_vec[curr_depth].right = curr_edge_index;
    visited[curr_vert] = true;
    int next_neighbor_index = lookup->offset_vec[curr_vert] + curr_edge_index;
    if (next_neighbor_index >= lookup->offset_vec[curr_vert + 1]) {
      cvector_pop_back(pos_stack_vec);
      continue;
    }
    edge_offsets[curr_vert]++;
    if (dfs_flow->edge_flow[next_neighbor_index])
      continue;
    // update state
    int next_neighbor = lookup->others_vec[next_neighbor_index];
    if (visited[next_neighbor]) {
      continue;
    }
    if (next_neighbor == sink) {
      for (int i = 0; i < cvector_size(pos_stack_vec); i++) {
        int used_edge = pos_stack_vec[i].right;
        int used_vert = pos_stack_vec[i].left;
        int used_edge_offset = lookup->offset_vec[used_vert] + used_edge;
        assert(used_edge != -1);
        assert(dfs_flow->edge_flow[used_edge_offset] == false);
        int reverse_edge_offset = lookup->reverse_edge_vec[used_edge_offset];
        if (dfs_flow->edge_flow[reverse_edge_offset]) {
          dfs_flow->edge_flow[reverse_edge_offset] = false;
        } else {
          dfs_flow->edge_flow[used_edge_offset] = true;
        }
      }
      cvector_clear(pos_stack_vec);
      cvector_push_back(pos_stack_vec, initial_state);
      found = true;
      break;
    }

    LLTuple nn_tup = {.left = next_neighbor, .right = -1};
    cvector_push_back(pos_stack_vec, nn_tup);
  }
  int n_visited = 0;
  for (int i = 0; i < num_verts; i++)
    n_visited += visited[i];

  free(edge_offsets);
  free(visited);
  cvector_free(pos_stack_vec);

  return (LLTuple){found, n_visited};
}

typedef struct {
  int max_flow;
  int still_connected_count;
} DFSFlowResult;

DFSFlowResult dfs_flow(NumberedEdgeLookup *const lookup, int source, int sink) {
  int flow = 0;
  DFSFlow dfs_flow = {NULL};
  dfs_flow.edge_flow = malloc(sizeof(bool) * cvector_size(lookup->others_vec));
  for (int i = 0; i < cvector_size(lookup->others_vec); i++)
    dfs_flow.edge_flow[i] = false;

  while (true) {
    LLTuple res = augment_flow(lookup, &dfs_flow, source, sink);
    if (!res.left) {
      return (DFSFlowResult){.max_flow = flow,
                             .still_connected_count = res.right};
    }
    flow++;
  }
}

LLTuple year23_day25(char *buf, long buf_len) {
  LLTuple res = {0};
  LabelledOffsetGenerator gen = generator_init();
  LLTuple *edge_vec = NULL;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    char *source_node = strsep(&line, ":");
    int source_offset = get_or_create_offset(&gen, hash_string(source_node));
    line++; // skip first whitespace
    while (line != NULL) {
      char *other_node = strsep(&line, " ");
      int other_offset = get_or_create_offset(&gen, hash_string(other_node));
      LLTuple edge1 = {.left = source_offset, .right = other_offset};
      LLTuple edge2 = {.right = source_offset, .left = other_offset};
      cvector_push_back(edge_vec, edge1);
      cvector_push_back(edge_vec, edge2);
    }
  }

  NumberedEdgeLookup edge_lookup = create_edge_lookup(edge_vec);

  const int num_verts = cvector_size(edge_lookup.offset_vec) - 1;
  bool done = false;

  // parallel not worth, since we hit on second attempt
  // #pragma omp parallel for
  for (int sink = 1; sink < cvector_size(edge_lookup.offset_vec) - 1; sink++) {
    if (done)
      continue;
    DFSFlowResult flow_res = dfs_flow(&edge_lookup, 0, sink);
    if (flow_res.max_flow == 3) {
      // #pragma omp critical
      done = true;
      res.left = (num_verts - flow_res.still_connected_count) *
                 flow_res.still_connected_count;
    }
  }

  return res;
}
AocDayRes solve_year23_day25(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day25(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
