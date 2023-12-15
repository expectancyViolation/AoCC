#include "day15.h"
#include "../../res/hashmap.c/hashmap.h"
#include "../util/parallelize.h"

#define MAX_ITEM_SIZE 100

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
  char content[MAX_ITEM_SIZE];
  long value;
  long insertion_step;
} Item;

int item_compare(const void *a, const void *b, void *udata) {
  const Item *ia = a;
  const Item *ib = b;
  return strcmp(ia->content, ib->content);
}
u_int64_t item_hash(const void *item, u_int64_t seed0, u_int64_t seed1) {
  const Item *res = item;
  return res->hash;
}

int item_insertion_step_compare(const void *a, const void *b, void *udata) {
  const Item *ia = a;
  const Item *ib = b;
  return CMP(ia->insertion_step, ib->insertion_step);
}

void print_item(const Item *item) {
  printf("item:\n\tcontent: %s\n\tvalue: %lld\t\n hash:%hhu\n\t(first "
         "inserted: %ld)\n",
         item->content, item->value, item->hash, item->insertion_step);
}


long long score_box(Item *curr_box) {
  long long res = 0;
  qsort(curr_box, cvector_size(curr_box), sizeof(Item),
        (__compar_fn_t)item_insertion_step_compare);
  for (int i = 0; i < cvector_size(curr_box); i++) {
    res += (i + 1) * curr_box[i].value;
  }
  return res;
}

// hashmap does not maintain insertion order => sort by "first inserted"
long long score_lenses(struct hashmap *map) {
  long long res = 0;

  size_t iter = 0;
  void *item;
  unsigned char prev_hash = 0;
  Item *curr_box = NULL;
  while (hashmap_iter(map, &iter, &item)) {
    Item *item1 = item;
    if (prev_hash == item1->hash) {
      cvector_push_back(curr_box, *item1);
    } else {
      int box_score = ((int)(prev_hash)) + 1;
      res += box_score * score_box(curr_box);
      cvector_clear(curr_box);
      cvector_push_back(curr_box, *item1);
      prev_hash = item1->hash;
    }
  }
  int box_score = ((int)(prev_hash)) + 1;
  res += box_score * score_box(curr_box);
  return res;
}

LLTuple year23_day15(char *buf, long buf_len) {
  LLTuple res = {0};

  struct hashmap *map =
      hashmap_new(sizeof(Item), 0, 0, 0, item_hash, item_compare, NULL, NULL);
  int cnt = 0;
  while (buf != NULL) {
    cnt += 1;
    // assert(cnt <100);
    char *line = strsep(&buf, ",");
    const unsigned char line_hash = hash(line);
    res.left = res.left + line_hash;
    char *remove_pos = strchr(line, '-');
    if (remove_pos != NULL) {
      // handle remove
      line[remove_pos - line] = 0;
      Item item = {.hash = hash(line)};
      strcpy(item.content, line);
      hashmap_delete(map, &item);
    } else {
      char *label = strsep(&line, "=");

      long value = strtol(line, NULL, 10);
      Item item = {.hash = hash(label), .value = value, .insertion_step = cnt};
      strcpy(item.content, label);
      const Item *replaced = hashmap_set(map, &item);
      if (replaced != NULL) {
        // weird hack reinsert with _old_ "first inserted
        item.insertion_step = replaced->insertion_step;
        hashmap_set(map, &item);
      }
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
