#include "day15.h"
#include "../../res/hashmap.c/hashmap.h"
#include "../util/parallelize.h"

#define MAX_ITEM_SIZE 40

unsigned char hash(char *input) {
  unsigned char res = 0;
  while (*input != 0) {
    res = (res + (*input)) * 17;
    input++;
  }
  return res;
}

typedef struct {
  unsigned char hash;
  //  char content[MAX_ITEM_SIZE];
  char *content;
  uint64_t content_hash;
  long value;
  long insertion_step;
} Item;

int item_compare(const void *a, const void *b, void *udata) {
  const Item *ia = a;
  const Item *ib = b;
  return CMP(ia->content_hash, ib->content_hash);
}
u_int64_t item_hash(const void *item, u_int64_t seed0, u_int64_t seed1) {
  const Item *res = item;
  return res->content_hash;
}

int item_insertion_step_compare(const void *a, const void *b, void *udata) {
  const Item *ia = a;
  const Item *ib = b;
  const int hash_compare = CMP(ia->hash, ib->hash);
  if (hash_compare != 0)
    return hash_compare;
  return CMP(ia->insertion_step, ib->insertion_step);
}

void print_item(const Item *item) {
  printf("item:\n\tcontent: %s\n\tvalue: %ld\t\n hash:%hhu\n\t(first "
         "inserted: %ld)\n",
         (item->content), item->value, item->hash, item->insertion_step);
}

static void print_all_entries(struct hashmap *map) {
  size_t iter = 0;
  void *item;
  printf("ENTRIES:\n");
  while (hashmap_iter(map, &iter, &item)) {
    const Item *item1 = item;
    print_item(item1);
  }
}

long long score_lenses_vec(Item *all_lenses) {
  long long res = 0;
  qsort(all_lenses, cvector_size(all_lenses), sizeof(Item),
        (__compar_fn_t)item_insertion_step_compare);
  int prev_hash = -1;
  int hash_run = 0;
  for (int i = 0; i < cvector_size(all_lenses); i++) {
    Item curr_lens = all_lenses[i];
    // print_item(&curr_lens);
    if (curr_lens.hash != prev_hash) {
      hash_run = 0;
      prev_hash = curr_lens.hash;
    } else {
      hash_run += 1;
    }
    long long score =
        (((int)(curr_lens.hash)) + 1) * (hash_run + 1) * curr_lens.value;
    res += score;
  }
  return res;
}

long long score_lenses(struct hashmap *map) {
  size_t iter = 0;
  void *item;
  Item *all_lenses = NULL;
  while (hashmap_iter(map, &iter, &item)) {
    Item *item1 = item;
    cvector_push_back(all_lenses, *item1);
  }
  return score_lenses_vec(all_lenses);
}

LLTuple year23_day15(char *buf, long buf_len) {
  LLTuple res = {0};

  struct hashmap *map =
      hashmap_new(sizeof(Item), 0, 0, 0, item_hash, item_compare, NULL, NULL);
  int cnt = 0;
  while (buf != NULL) {
    cnt += 1;
    char *line = strsep(&buf, ",");
    const unsigned char line_hash = hash(line);
    res.left = res.left + line_hash;
    char *remove_pos = strchr(line, '-');
    if (remove_pos != NULL) {
      // handle remove
      line[remove_pos - line] = 0;
      const size_t line_len = remove_pos - line;
      uint64_t hash_val = hashmap_sip(line, (sizeof(char)) * line_len, 0, 0);
      Item item = {.hash = hash(line), .content_hash = hash_val};
      hashmap_delete(map, &item);
    } else {
      char *label = strsep(&line, "=");

      const size_t label_len = line - label - 1;

      uint64_t hash_val = hashmap_sip(label, (sizeof(char)) * label_len, 0, 0);
      long value = strtol(line, NULL, 10);
      Item item = {.hash = hash(label),
                   .value = value,
                   .insertion_step = cnt,
                   .content = label,
                   .content_hash = hash_val};
      const Item *to_replace = hashmap_get(map, &item);
      if (to_replace != NULL)
        item.insertion_step = to_replace->insertion_step;
      hashmap_set(map, &item);
    }
  }
  long long score = score_lenses(map);
  res.right = score;
  return res;
}

AocDayRes solve_year23_day15(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);

  LLTuple res = year23_day15(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
