#include "day20.h"
#include "../../res/hashmap.c/hashmap.h"
#include <stdint-gcc.h>

#define MAX_OUTPUTS 10
#define MAX_INPUTS 20
#define MAX_RULE_KEY_LEN 20
#define RINGBUFFER_MAX_SIZE 100000
#define MODULE_OFFSET_UNDEFINED (-1)

enum MODULE_TYPE {
  MODULE_TYPE_undefined = 0,
  MODULE_TYPE_flip_flop = 1,
  MODULE_TYPE_conjunction = 2,
  MODULE_TYPE_broadcast = 3
};

enum MODULE_TYPE parse_module_type(char const *line) {
  switch (line[0]) {
  case 'b':
    return MODULE_TYPE_broadcast;
  case '%':
    return MODULE_TYPE_flip_flop;
  case '&':
    return MODULE_TYPE_conjunction;
  default:
    assert(false);
  }
}

typedef struct {
  enum MODULE_TYPE module_type;
  int output_indices[MAX_OUTPUTS];
  int output_count;
  int input_indices[MAX_INPUTS];
  int input_count;
  bool memory[MAX_OUTPUTS];
} D20Module;

static const D20Module DUMMY_MODULE = {
    .module_type = MODULE_TYPE_undefined,
    .output_indices = {[0 ...(MAX_OUTPUTS - 1)] = MODULE_OFFSET_UNDEFINED},
    .output_count = 0,
    .input_indices = {[0 ...(MAX_INPUTS - 1)] = MODULE_OFFSET_UNDEFINED},
    .input_count = 0,
};

typedef struct {
  char module_key[MAX_RULE_KEY_LEN + 1];
  int rule_offset;
} D20ModuleOffset;

static int d20rule_offset_compare(const void *a, const void *b, void *udata) {
  const D20ModuleOffset *sa = a;
  const D20ModuleOffset *sb = b;
  return strcmp(sa->module_key, sb->module_key);
}

static u_int64_t d20rule_offset_hash(const void *item, u_int64_t seed0,
                                     u_int64_t seed1) {
  const D20ModuleOffset *res = item;
  uint64_t hash_val =
      hashmap_sip(res->module_key, strlen(res->module_key), 0, 0);
  return hash_val;
}

typedef struct {
  struct hashmap *module_offsets;
  D20Module *module_vec;
  int broadcaster_offset;
  int rx_input_offset;
} D20ModuleLookup;

int get_or_create_offset(D20ModuleLookup *lookup, char *module_key) {
  D20ModuleOffset search = {0};
  strcpy(search.module_key, module_key);
  D20ModuleOffset *offset = hashmap_get(lookup->module_offsets, &search);
  if (offset == NULL) {
    const int res = cvector_size(lookup->module_vec);
    cvector_push_back(lookup->module_vec, DUMMY_MODULE);
    search.rule_offset = res;
    hashmap_set(lookup->module_offsets, &search);
    return res;
  }
  return offset->rule_offset;
}

void parse_module(char *line, D20ModuleLookup *lookup) {
  char *module_key = strsep(&line, " ");
  enum MODULE_TYPE type = parse_module_type(module_key);
  if (type != MODULE_TYPE_broadcast)
    module_key++;
  int offset = get_or_create_offset(lookup, module_key);
  if (type == MODULE_TYPE_broadcast)
    lookup->broadcaster_offset = offset;
  D20Module module = lookup->module_vec[offset];
  module.module_type = type;
  line += 2; // skip "->"
  while (line != NULL) {
    char *output_key = strsep(&line, ",");
    output_key++; // skip whitespace
    int target_offset = get_or_create_offset(lookup, output_key);
    assert(module.output_count < MAX_OUTPUTS);
    module.output_indices[module.output_count] = target_offset;
    module.output_count++;
    // add module to target input (ref is safe since no write to module_vec)
    D20Module *target = &(lookup->module_vec[target_offset]);
    assert(target->input_count < MAX_INPUTS);
    target->input_indices[target->input_count] = offset;
    (target->input_count)++;
    if (strcmp(output_key, "rx") == 0) {
      lookup->rx_input_offset = target_offset;
    }
  }
  lookup->module_vec[offset] = module;
}

typedef struct {
  int target_offset;
  bool high;
  int sender_offset;
} D20Pulse;

typedef struct {
  D20Pulse buffer[RINGBUFFER_MAX_SIZE];
  int head_offset;
  int tail_offset;
} D20Ringbuffer;

static void ringbuffer_push(D20Ringbuffer *ring, D20Pulse el) {
  assert((ring->tail_offset + 1) != ring->head_offset);
  ring->buffer[ring->tail_offset] = el;
  ring->tail_offset = (ring->tail_offset + 1) % RINGBUFFER_MAX_SIZE;
}

static D20Pulse ringbuffer_pop(D20Ringbuffer *ring) {
  assert(ring->head_offset != ring->tail_offset);
  const D20Pulse res = ring->buffer[ring->head_offset];
  ring->head_offset = (ring->head_offset + 1) % RINGBUFFER_MAX_SIZE;
  return res;
}

static bool ringbuffer_is_empty(D20Ringbuffer const *ring) {
  return (ring->head_offset) == (ring->tail_offset);
}

static int ringbuffer_size(D20Ringbuffer const *ring) {
  return (RINGBUFFER_MAX_SIZE + ring->tail_offset - ring->head_offset) %
         RINGBUFFER_MAX_SIZE;
}

LLTuple year23_day20(char *buf, long buf_len) {
  LLTuple res = {0};
  struct hashmap *module_lookup =
      hashmap_new(sizeof(D20ModuleOffset), 0, 0, 0, d20rule_offset_hash,
                  d20rule_offset_compare, NULL, NULL);
  D20ModuleLookup lookup = {.module_vec = NULL,
                            .module_offsets = module_lookup,
                            .broadcaster_offset = MODULE_OFFSET_UNDEFINED};
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    parse_module(line, &lookup);
  }

  // monitor input modules of last conj that feeds into rx
  // this only works b.c. of special input structure:
  // disjoint parts that are fed by button
  //  and themselves feed into final conj
  D20Module *rx = &(lookup.module_vec[lookup.rx_input_offset]);
  D20Module *rx_pre = &(lookup.module_vec[rx->input_indices[0]]);

  int *rx_pre_input_periods = malloc(rx_pre->input_count);
  for (int i = 0; i < rx_pre->input_count; i++)
    rx_pre_input_periods[i] = 0;

  bool *pulse_states = NULL;
  for (int i = 0; i < cvector_size(lookup.module_vec); i++) {
    cvector_push_back(pulse_states, false);
  }

  long long low_pulse_count = 0;
  long long high_pulse_count = 0;
  D20Ringbuffer buffer = {.head_offset = 0, .tail_offset = 0};
  for (long long n_push = 1; n_push < 10000; n_push++) {
    int push_low_pulse_count = 0;
    int push_high_pulse_count = 0;
    ringbuffer_push(
        &buffer, (D20Pulse){.target_offset = lookup.broadcaster_offset, false});
    while (!ringbuffer_is_empty(&buffer)) {
      D20Pulse pulse = ringbuffer_pop(&buffer);
      if (pulse.high)
        push_high_pulse_count += 1;
      else
        push_low_pulse_count += 1;
      // handle pulse
      D20Module *curr_mod = &(lookup.module_vec[pulse.target_offset]);
      bool result_high = false;
      bool do_output = true;
      switch (curr_mod->module_type) {
      case MODULE_TYPE_broadcast:
        result_high = false;
        break;
      case MODULE_TYPE_conjunction: {
        bool conj_res = true;
        for (int i = 0; i < curr_mod->input_count; i++) {
          conj_res &= pulse_states[curr_mod->input_indices[i]];
        }
        result_high = !conj_res;
        break;
      }
      case MODULE_TYPE_flip_flop: {
        bool curr_state = pulse_states[pulse.target_offset];
        result_high = (pulse.high) ? curr_state : (!curr_state);
        do_output = !pulse.high;
        break;
      }
      default:
        do_output = false;
      }
      if (do_output) {
        pulse_states[pulse.target_offset] = result_high;
        for (int i = 0; i < curr_mod->output_count; i++) {
          const int target_offset = curr_mod->output_indices[i];
          ringbuffer_push(&buffer,
                          (D20Pulse){.target_offset = target_offset,
                                     .high = result_high,
                                     .sender_offset = pulse.target_offset});
        }
      }
      if (result_high) {
        for (int i = 0; i < rx_pre->input_count; i++) {
          if (rx_pre_input_periods[i] != 0)
            continue;
          if (pulse.target_offset == rx_pre->input_indices[i]) {
            printf("found period:%d at time push %d\n", i, n_push);
            rx_pre_input_periods[i] = n_push;
          }
        }
      }
    }
    if (n_push <= 1000) {
      low_pulse_count += push_low_pulse_count;
      high_pulse_count += push_high_pulse_count;
    }
  }

  res.left = low_pulse_count * high_pulse_count;

  // lcm of periods
  res.right = 1;
  for (int i = 0; i < rx_pre->input_count; i++)
    res.right = (res.right * rx_pre_input_periods[i]) /
                gcd(res.right, rx_pre_input_periods[i]);

  return res;
}
AocDayRes solve_year23_day20(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day20(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
