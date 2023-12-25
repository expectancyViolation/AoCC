#include "day19.h"
#include "../../res/hashmap.c/hashmap.h"
#include <stdint-gcc.h>

#define MAX_RULE_COUNT 10

#define MAX_RULE_KEY_LEN 10

#define RULE_OFFSET_UNSET -1

enum D19_TARGET_TYPE {
  D19_TARGET_TYPE_none = 0,
  D19_TARGET_TYPE_terminator = 1,
  D19_TARGET_TYPE_rule = 2
};

enum D19_CHAR_OFFSET {
  D19_CHAR_OFFSET_x = 0,
  D19_CHAR_OFFSET_m = 1,
  D19_CHAR_OFFSET_a = 2,
  D19_CHAR_OFFSET_s = 3
};

#define NUM_D19_CHAR_OFFSET 4

typedef struct {
  enum D19_TARGET_TYPE target_type;
  union {
    bool accept;
    int target_rule_offset;
  };
} D19Target;

void print_d19target(const D19Target *tar) {
  switch (tar->target_type) {
  case D19_TARGET_TYPE_terminator:
    printf("tar term:%d\n", tar->accept);
    return;
  case D19_TARGET_TYPE_rule:
    printf("tar rule:%d\n", tar->target_rule_offset);
    break;
  default:
    printf("tar UNSET! %d\n", tar->target_type);
  }
}

typedef struct {
  bool is_greater;
  enum D19_CHAR_OFFSET split_char_offset;
  long split_val;
  D19Target target;
} D19SplitRule;

void print_d19split_rule(const D19SplitRule *rule) {
  printf("split rule: %d %ld %d", rule->is_greater, rule->split_val,
         rule->split_char_offset);
  printf("\n");
  print_d19target(&(rule->target));
}

typedef struct {
  char rule_key[MAX_RULE_KEY_LEN + 1];
  D19SplitRule split_rules[MAX_RULE_COUNT];
  int num_split_rules;
  D19Target default_target;
} D19Rule;

void print_d19rule(const D19Rule *rule) {
  printf("RULE:\nkey:%s\n", rule->rule_key);
  int c = 0;
  for (int i = 0; i < MAX_RULE_COUNT; i++) {
    c = i;
    const D19SplitRule *split_rule = &(rule->split_rules[i]);
    if (split_rule->target.target_type == D19_TARGET_TYPE_none)
      break;
    print_d19split_rule(split_rule);
  }
  assert(c == rule->num_split_rules);
  printf("DEFAULT:\n");
  print_d19target(&(rule->default_target));
  printf("\n");
}

typedef struct {
  char rule_key[MAX_RULE_KEY_LEN + 1];
  int rule_offset;
} D19RuleOffset;

static int d19rule_offset_compare(const void *a, const void *b, void *udata) {
  const D19RuleOffset *sa = a;
  const D19RuleOffset *sb = b;
  return strcmp(sa->rule_key, sb->rule_key);
}

static u_int64_t d19rule_offset_hash(const void *item, u_int64_t seed0,
                                     u_int64_t seed1) {
  const D19RuleOffset *res = item;
  uint64_t hash_val = hashmap_sip(res->rule_key, strlen(res->rule_key), 0, 0);
  return hash_val;
}

int get_or_create_target_rule(struct hashmap *rule_lookup, D19Rule **rules_vec,
                              D19RuleOffset *search_rule) {

  D19RuleOffset *curr_rule_offset = hashmap_get(rule_lookup, search_rule);
  if (curr_rule_offset == NULL) {
    D19Rule new_rule = {.num_split_rules = 0};
    strcpy(new_rule.rule_key, search_rule->rule_key);
    search_rule->rule_offset = cvector_size(*rules_vec);
    cvector_push_back(*rules_vec, new_rule);
    hashmap_set(rule_lookup, search_rule);
    return search_rule->rule_offset;
  }
  return curr_rule_offset->rule_offset;
}

D19Target parse_target(const char *target_string) {
  D19Target res = {0};
  switch (target_string[0]) {
  case 'A':
  case 'R':
    res.target_type = D19_TARGET_TYPE_terminator;
    res.accept = target_string[0] == 'A';
    break;
  default:
    res.target_type = D19_TARGET_TYPE_rule;
    res.target_rule_offset = RULE_OFFSET_UNSET;
  }
  return res;
}

D19Target const *match_rule_target(D19Rule const *curr_rule, long const *xmas) {
  for (int i = 0; i < MAX_RULE_COUNT; i++) {
    const D19SplitRule *curr_split_rule = &(curr_rule->split_rules[i]);
    if (curr_split_rule->target.target_type == D19_TARGET_TYPE_none)
      break;
    const long test_val = xmas[curr_split_rule->split_char_offset];
    bool rule_matched = (curr_split_rule->is_greater)
                            ? (test_val > curr_split_rule->split_val)
                            : (test_val < curr_split_rule->split_val);
    if (rule_matched) {
      return &(curr_split_rule->target);
    }
  }
  return &(curr_rule->default_target);
}

bool judge_xmas(D19Rule const *rules_vec, int in_rule_offset,
                long const *xmas) {
  D19Target const initial_target = {.target_type = D19_TARGET_TYPE_rule,
                                    .target_rule_offset = in_rule_offset};
  D19Target const *curr_target = &initial_target;
  while (curr_target->target_type != D19_TARGET_TYPE_terminator) {
    const D19Rule *rule = &(rules_vec[curr_target->target_rule_offset]);
    curr_target = match_rule_target(rule, xmas);
  }
  return curr_target->accept;
}

bool advance_comb(int *combs, int const *lims) {
  for (int i = 3; i >= 0; i--) {
    if (combs[i] < lims[i] - 1) {
      combs[i]++;
      return false;
    }
    combs[i] = 0;
  }
  return true;
}

enum D19_STATUS {
  D19_STATUS_undefined = 0,
  D19_STATUS_matching = 1,
  D19_STATUS_accepted = 2,
  D19_STATUS_rejected = 3
};

typedef struct {
  long long upper_bounds[NUM_D19_CHAR_OFFSET];
  long long lower_bounds[NUM_D19_CHAR_OFFSET];
  int rule_num;
  int split_rule_offset;
  enum D19_STATUS status;
} D19State;

void print_d19_state(D19State const *state) {
  printf("state:\n\trule:%d\n\toffset:%d\n\tstatus:%d\n", state->rule_num,
         state->split_rule_offset, state->status);
  for (int i = 0; i < 4; i++) {
    printf("%d to %d\n", state->lower_bounds[i], state->upper_bounds[i]);
  }
}

bool apply_split_rule(D19Rule const *rules_vec, D19State *state, bool invert) {
  D19Rule const *curr_rule = &(rules_vec[state->rule_num]);
  assert(state->status == D19_STATUS_matching);
  assert(state->split_rule_offset < MAX_RULE_COUNT);
  if (state->split_rule_offset > curr_rule->num_split_rules)
    return false;
  const bool at_default_rule =
      (state->split_rule_offset) == curr_rule->num_split_rules;
  D19Target const *target = NULL;
  if (at_default_rule) {
    if (invert)
      return false;
    target = &(curr_rule->default_target);
  } else {
    D19SplitRule const *curr_split_rule =
        &(curr_rule->split_rules[state->split_rule_offset]);
    target = &(curr_split_rule->target);
    if (curr_split_rule->is_greater) {
      if (invert) {
        state->upper_bounds[curr_split_rule->split_char_offset] =
            min(state->upper_bounds[curr_split_rule->split_char_offset],
                (curr_split_rule->split_val) + 1);
      } else {
        state->lower_bounds[curr_split_rule->split_char_offset] =
            max(state->lower_bounds[curr_split_rule->split_char_offset],
                (curr_split_rule->split_val) + 1);
      }
    } else {
      if (invert) {
        state->lower_bounds[curr_split_rule->split_char_offset] =
            max(state->lower_bounds[curr_split_rule->split_char_offset],
                (curr_split_rule->split_val));
      } else {
        state->upper_bounds[curr_split_rule->split_char_offset] =
            min(state->upper_bounds[curr_split_rule->split_char_offset],
                curr_split_rule->split_val);
      }
    }
  }
  if (invert)
    return true;

  if (target->target_type == D19_TARGET_TYPE_terminator) {
    state->status =
        (target->accept) ? D19_STATUS_accepted : D19_STATUS_rejected;
  } else {
    assert(target->target_type == D19_TARGET_TYPE_rule);
    state->rule_num = target->target_rule_offset;
    state->split_rule_offset = 0;
  }
  return true;
}

LLTuple year23_day19(char *buf, long buf_len) {
  LLTuple res = {0};

  struct hashmap *rule_lookup =
      hashmap_new(sizeof(D19RuleOffset), 0, 0, 0, d19rule_offset_hash,
                  d19rule_offset_compare, NULL, NULL);

  D19Rule *rules_vec = NULL;
  int in_rule_offset = -1;

  D19RuleOffset search_rule = {.rule_key = "", .rule_offset = -1};

  while (buf[0] != '\n') {
    char *line = strsep(&buf, "\n");
    char *rule_key = strsep(&line, "{");
    strcpy(search_rule.rule_key, rule_key);
    int offset =
        get_or_create_target_rule(rule_lookup, &rules_vec, &search_rule);
    if (strcmp(rule_key, "in") == 0) {
      in_rule_offset = offset;
    }
    D19Rule curr_rule = (rules_vec[offset]);

    char *rules = strsep(&line, "}");
    while (rules != NULL) {
      char *split_rule_string = strsep(&rules, ",");
      D19SplitRule *curr_split_rule =
          &(curr_rule.split_rules[curr_rule.num_split_rules]);
      bool is_default_target =
          (split_rule_string[1] != '<') && (split_rule_string[1] != '>');
      if (is_default_target) {
        const char *target = split_rule_string;
        curr_rule.default_target = parse_target(target);
        if (curr_rule.default_target.target_type == D19_TARGET_TYPE_rule) {
          strcpy(search_rule.rule_key, target);
          curr_rule.default_target.target_rule_offset =
              get_or_create_target_rule(rule_lookup, &rules_vec, &search_rule);
        }
        break;
      } else {
        curr_rule.num_split_rules++;
      }
      const char *xmas = "xmas";
      curr_split_rule->split_char_offset =
          strchr(xmas, split_rule_string[0]) - xmas;
      curr_split_rule->is_greater = split_rule_string[1] == '>';
      char *end_ptr = split_rule_string + 2;
      curr_split_rule->split_val = strtol(end_ptr, &end_ptr, 10);
      char *target_key = end_ptr + 1;
      curr_split_rule->target = parse_target(target_key);
      if (curr_split_rule->target.target_type == D19_TARGET_TYPE_rule) {
        strcpy(search_rule.rule_key, target_key);
        const int offs =
            get_or_create_target_rule(rule_lookup, &rules_vec, &search_rule);
        curr_split_rule->target.target_rule_offset = offs;
      }
    }
    rules_vec[offset] = curr_rule;
  }
  strsep(&buf, "\n"); // skip empty line
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    line++; // skip bracket
    long xmas_val[4] = {-1, -1, -1, -1};
    for (int i = 0; i < 4; i++) {
      char *term = strsep(&line, ",");
      xmas_val[i] = strtol(term + 2, NULL, 10);
    }
    if (judge_xmas(rules_vec, in_rule_offset, xmas_val)) {
      for (int i = 0; i < 4; i++) {
        res.left += xmas_val[i];
      }
    }
  }

  D19State *state_stack = NULL;
  D19State initial_state = {.upper_bounds = {4001, 4001, 4001, 4001},
                            .lower_bounds = {1, 1, 1, 1},
                            .split_rule_offset = 0,
                            .status = D19_STATUS_matching,
                            .rule_num = in_rule_offset};
  cvector_push_back(state_stack, initial_state);
  long long total_vol = 0;
  while (cvector_size(state_stack) > 0) {
    D19State curr_state = state_stack[cvector_size(state_stack) - 1];
    if (curr_state.status != D19_STATUS_matching) {
      if(curr_state.status==D19_STATUS_accepted) {
        long long curr_vol = 1;
        for (int i = 0; i < NUM_D19_CHAR_OFFSET; i++) {
          curr_vol *= (curr_state.upper_bounds[i] - curr_state.lower_bounds[i]);
        }
        total_vol += curr_vol;
      }
      cvector_pop_back(state_stack);
      continue;
    }
    bool could_step = apply_split_rule(rules_vec, &curr_state, false);
    apply_split_rule(rules_vec, &state_stack[cvector_size(state_stack) - 1],
                     true);
    state_stack[cvector_size(state_stack) - 1].split_rule_offset++;
    if (!could_step) {
      cvector_pop_back(state_stack);
      continue;
    }
    cvector_push_back(state_stack, curr_state);
  }

  res.right =total_vol;

  return res;
}
AocDayRes solve_year23_day19(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day19(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
