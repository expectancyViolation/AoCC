#ifdef DB_USE_IN_MEMORY
#include "result_db.h"

#include "../../res/hashmap.c/hashmap.h"

#define DUMPFILE "/tmp/aoc_db.bin"
#define DUMPCOUNT_LIMIT 1000

int result_status_key(const ResultStatus *status) {
  return (status->year) * 1000 + (status->day) * 10 + status->part;
}

int result_status_compare(const void *a, const void *b, void *udata) {
  const ResultStatus *sa = a;
  const ResultStatus *sb = b;

  const int key_a = result_status_key(sa);
  const int key_b = result_status_key(sb);
  return CMP(key_a, key_b);
}

uint64_t result_status_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const ResultStatus *status = item;
  const int key = result_status_key(status);
  const uint64_t h = hashmap_sip(&key, sizeof(key), seed0, seed1);
  return h;
}

typedef struct _DbHandleDataInMemory {
  struct hashmap *map;
  bool modified;
} DbHandleDataInMemory;

DbHandleDataInMemory *db_handle_deref_handle(result_db_handle handle) {
  return (DbHandleDataInMemory *)handle;
}

static void result_db_print_all_entries(result_db_handle handle) {
  DbHandleDataInMemory *handleData = db_handle_deref_handle(handle);
  size_t iter = 0;
  void *item;
  while (hashmap_iter(handleData->map, &iter, &item)) {
    printf("lul");
    const ResultStatus *entry = item;
    print_result_status(entry);
  }
}

void load_dump(const char *dumpfile, DbHandleDataInMemory *handle_data) {
  FILE *f = fopen(dumpfile, "rb");
  if (f == NULL) {
    printf("failed to load dump :(\n");
    return;
  }
  ResultStatus *results = malloc((sizeof *results) * DUMPCOUNT_LIMIT);
  const size_t read = fread(results, sizeof(ResultStatus), DUMPCOUNT_LIMIT, f);
  for (size_t i = 0; i < read; i++) {
    hashmap_set(handle_data->map, &(results[i]));
  }
  fclose(f);
  printf("loaded dump of size %ld.\n\n", read);
  free(results);
}

void dump(const char *dumpfile, DbHandleDataInMemory const *handle_data) {
  if (!(handle_data->modified)) {
    printf("skipping dump (unmodified).");
    return;
  }
  FILE *f = fopen(dumpfile, "wb");
  ResultStatus *results = malloc((sizeof *results) * DUMPCOUNT_LIMIT);
  size_t iter = 0;
  void *item;
  while (hashmap_iter(handle_data->map, &iter, &item)) {
    if (iter >= DUMPCOUNT_LIMIT)
      break;
    const ResultStatus *entry = item;
    results[iter] = *entry;
  }
  const size_t written = fwrite(results, sizeof(ResultStatus), iter, f);
  fclose(f);
  free(results);
  assert(written == iter);
}

result_db_handle result_db_init_db(char *db_file) {
  DbHandleDataInMemory *handle_data = malloc(sizeof(*handle_data));
  handle_data->modified = false;
  handle_data->map =
      hashmap_new(sizeof(ResultStatus), 0, 0, 0, result_status_hash,
                  result_status_compare, NULL, NULL);
  load_dump(DUMPFILE, handle_data);
  return (result_db_handle)handle_data;
}

void result_db_close(result_db_handle handle) {
  DbHandleDataInMemory *handle_data = db_handle_deref_handle(handle);
  dump(DUMPFILE, handle_data);
  hashmap_free(handle_data->map);
  free(handle_data);
}

bool result_status_load_entry(result_db_handle handle, int year, int day,
                              enum AOC_DAY_PART part, ResultStatus **out) {
  DbHandleDataInMemory *handleData = db_handle_deref_handle(handle);
  *out = (ResultStatus *)(hashmap_get(
      handleData->map,
      &(ResultStatus){.year = year, .day = day, .part = part}));
  return (*out) != NULL;
}

bool result_status_store_entry(result_db_handle handle,
                               const ResultStatus *status, bool overwrite) {
  DbHandleDataInMemory *handle_data = db_handle_deref_handle(handle);
  // TODO: what does set return?
  hashmap_set(handle_data->map, status);
  handle_data->modified = true;
  return true;
}

void result_db_test() {
  result_db_handle handle = result_db_init_db("dummy");
  const int test_year = 2009;
  const int max_year = 1000000;
  printf("size:%ld\n", sizeof(ResultStatus));
  const int test_day = 12;
  const int test_part = AOC_DAY_PART_part2;
  char *solution = "hashMe!";
  for (int year = 0; year < max_year; year++) {
    ResultStatus test_status = {
        .year = year, .day = test_day, .part = test_part};
    // result_status_init(&test_status);
    test_status.solution_type = SOLUTION_TYPE_num;
    strncpy(test_status.submissions[0].string_solution, solution,
            AOC_SOL_MAX_LEN + 1);
    test_status.num_solution = (year & 787) % 3479;
    test_status.solved = true;
    result_status_store_entry(handle, &test_status, true);
  }

  printf("inserted\n");
  // result_db_print_all_entries(handle);
  srand(2323);
  int sol_sum = 0;
  for (int i = 0; i < 100 * 1000 * 1000; i++) {
    ResultStatus *test_status_2 = NULL;
    const int sample_year = i % max_year;
    result_status_load_entry(handle, sample_year, test_day, test_part,
                             &test_status_2);
    sol_sum += test_status_2->num_solution;
    assert(test_status_2 != NULL);
    assert(strcmp(test_status_2->submissions[0].string_solution, solution) ==
           0);
  }
  printf("sol sum:%d", sol_sum);

  result_db_close(handle);
};

#endif // DB_USE_IN_MEMORY
