#include "day19.h"
#include "../../res/hashmap.c/hashmap.h"
#include <stdint-gcc.h>

#define MAX_RULE_COUNT 10

#define MAX_RULE_KEY_LEN 10

enum D19_TARGET_TYPE {
  D19_TARGET_TYPE_none = 0,
  D19_TARGET_TYPE_terminator = 1,
  D19_TARGET_TYPE_rule = 2
};

typedef struct {
  bool is_greater;
  char split_char;
  enum D19_TARGET_TYPE target_type;
  union {
    char terminator;
    int target_rule_offset;
  };

} D19SplitRule;

typedef struct {
  char rule_key[MAX_RULE_KEY_LEN + 1];
  D19SplitRule split_rules[MAX_RULE_COUNT];
} D19Rule;

static int d19rule_compare(const void *a, const void *b, void *udata) {
  const D19Rule *sa = a;
  const D19Rule *sb = b;
  return strcmp(&(sa->rule_key), &(sb->rule_key));
}

static u_int64_t d19state_hash(const void *item, u_int64_t seed0,
                               u_int64_t seed1) {
  const D19Rule *res = item;
  uint64_t hash_val =
      hashmap_sip(&(res->rule_key), strlen(&(res->rule_key)), 0, 0);
  return hash_val;
}

LLTuple year23_day19(char *buf, long buf_len) {
  LLTuple res = {0};

  struct hashmap *rule_lookup = hashmap_new(
      sizeof(D19Rule), 0, 0, 0, d19state_hash, d19rule_compare, NULL, NULL);

  D19Rule *rules_vec = NULL;

  D19Rule search_rule = {0};

  while (buf[0] != '\n') {
    char *line = strsep(&buf, "\n");
    printf("rule%s\n", line);
    char *rule_key = strsep(&line, "{");
    strncpy(search_rule.rule_key, rule_key, MAX_RULE_KEY_LEN);
    D19Rule* curr_rule= hashmap_get(rule_lookup, &search_rule);
    if(curr_rule==NULL){
      search_rule.
      cvector_push_back(rules_vec,search_rule)
    }
    int i=0;
    while(*line!='}'){
      char * split_rule_string=strsep(&line,",");
      D19SplitRule * curr_split_rule=&(curr_rule.split_rules[i]);

    }
  }

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
