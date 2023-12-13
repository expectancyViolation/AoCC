#ifdef DB_USE_SQLITE
#include "result_db.h"

#include "aoc_solution_manager.h"

#include "../../res/asprintf.c/asprintf.h"

#include <assert.h>
#include <sqlite3.h>

typedef struct _DbHandleDataInMemory {
  sqlite3 *db;
} DbHandleDataInMemory;

DbHandleDataInMemory *db_handle_deref_handle(result_db_handle handle) {
  return (DbHandleDataInMemory *)handle;
}

const char *kv_create_query =
    "CREATE TABLE IF NOT EXISTS kv_store (key "
    "varchar(1023) PRIMARY KEY, value varchar(1023));";

result_db_handle result_db_init_db(char *db_file) {
  DbHandleDataInMemory *handle_data = NULL;
  sqlite3 *db;
  int rc;
  rc = sqlite3_open(db_file, &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  } else {
    char *errmsg = NULL;
    handle_data = malloc(sizeof(*handle_data));
    handle_data->db = db;
    sqlite3_exec(handle_data->db, kv_create_query, NULL, NULL, &errmsg);
    if (errmsg != NULL)
      printf("load error:%s\n", errmsg);
  }

  return (result_db_handle)handle_data;
}

void result_db_close(result_db_handle handle) {
  DbHandleDataInMemory *handle_data = db_handle_deref_handle(handle);
  if (handle_data != NULL) {
    printf("close:%d\n", sqlite3_close(handle_data->db));
  }
  free(handle_data);
}

// TODO: raw SQL 🤢
char *get_select_query(const char *key) {
  char *res;
  asprintf(&res, "select * from kv_store where key='%s';", key);
  return res;
}

int get_key_cb(ResultStatus **out, int ncols, char **cols, char **col_names) {
  for (int i = 0; i < ncols; i++) {
    // printf("%s:%s\n", col_names[i], cols[i]);
    if (strcmp(col_names[i], "value") == 0) {
      // printf("decoding:%s\n", cols[i]);
      *out = decode_result_status(cols[i]);
    }
  }
  return 0;
}

// TODO: raw SQL 🤢
char *get_insert_query(const char *key, const char *value, bool overwrite) {
  char *res;
  if (overwrite) {
    asprintf(&res,
             "insert or replace into kv_store (key,value) VALUES ('%s','%s');",
             key, value);
  } else {
    asprintf(&res, "insert into kv_store (key,value) VALUES ('%s','%s');", key,
             value);
  }
  return res;
}

bool result_status_load_entry(result_db_handle handle, int year, int day,
                              enum AOC_DAY_PART part, ResultStatus **out) {
  *out = NULL;
  DbHandleDataInMemory *handle_data = db_handle_deref_handle(handle);
  const char *key = result_status_combine_id(year, day, part);
  const char *query = get_select_query(key);
  char *errmsg = NULL;
  sqlite3_exec(handle_data->db, query,
               (int (*)(void *, int, char **, char **))get_key_cb, out,
               &errmsg);
  free(query);
  free(key);
  if (errmsg != NULL) {
    printf("load error:%s\n", errmsg);
    return false;
  }
  return *out != NULL;
}

bool result_status_store_entry(result_db_handle handle,
                               const ResultStatus *status, bool overwrite) {
  DbHandleDataInMemory *handle_data = db_handle_deref_handle(handle);
  char *key = result_status_get_id(status);
  char *value = encode_result_status(status);
  char *query = get_insert_query(key, value, overwrite);
  char *errmsg;
  sqlite3_exec(handle_data->db, query,
               (int (*)(void *, int, char **, char **))get_key_cb, NULL,
               &errmsg);
  if (errmsg != NULL) {
    printf("%s", errmsg);
    return false;
  }
  free(query);
  free(value);
  free(key);
  return true;
}

void result_db_test() {
  ResultStatus status;
  result_db_initialize_result_status(&status);
  status.num_solution_upper_bound = 2823;
  status.num_solution_lower_bound = 112;
  status.day = 1;
  status.part = AOC_DAY_PART_part1;
  // check decoding
  char *encoded = encode_result_status(&status);
  ResultStatus *decoded = decode_result_status(encoded);
  assert(memcmp(&status, decoded, sizeof status) == 0);
  free(encoded);
  free(decoded);

  // test insert and select
  char *db_file = "/tmp/testus_teron.db";
  result_db_handle handle = result_db_init_db(db_file);
  result_status_store_entry(handle, &status, true);
  ResultStatus *returned_status;
  result_status_load_entry(handle, status.year, status.day, status.part,
                           &returned_status);
  assert(memcmp(&status, returned_status, sizeof status) == 0);
  result_db_close(handle);
  free(returned_status);
}
#endif// DB_USE_SQLITE
