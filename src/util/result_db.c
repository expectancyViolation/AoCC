#include "result_db.h"

#include "../../res/b64.c/b64.h"
#include "aoc_solution_manager.h"

#include "../../res/asprintf.c/asprintf.h"

#include <assert.h>
#ifdef SQLITE_AVAILABLE
#include <sqlite3.h>
#endif

void print_result_status(const struct result_status *status) {
  assert(status != NULL);
  printf("status:\n"
         "\tyear:        %10d\n"
         "\tday:         %10d\n"
         "\tpart:        %10d\n"
         "\tsolved:      %10d\n",
         status->year, status->day, status->part, status->solved);
  printf("solution type: ");
  print_solution_type(status->solution_type);
  printf("\n");
  if (status->solved) {
    switch (status->solution_type) {
    case SOLUTION_TYPE_num:
      printf("\tsolution:    %10lld\n", status->num_solution);
      break;
    case SOLUTION_TYPE_string:
      printf("\tsolution:    %10s\n", status->string_solution);
      break;
    default:
      assert(false);
    }
  } else {
    printf("\tno solution so far :(\n");
  }
  if (status->solution_type == SOLUTION_TYPE_num) {
    printf("bounds\n"
           "\tlower_bound: %10lld\n"
           "\tupper_bound: %10lld\n",
           status->num_solution_lower_bound, status->num_solution_upper_bound);
  }
  printf("previous submissions:\n");
  // for (int i = 0; i < RESULT_STATUS_SUBMISSION_HISTORY_SIZE; i++) {
  for (int i = 0; i < 5; i++) {
    printf("%03d:%s\n", i, status->submissions[i].string_solution);
  }
  printf("....\n");
};

void print_solution_type(enum SOLUTION_TYPE sol) {
  switch (sol) {
  case SOLUTION_TYPE_num:
    printf("number");
    return;
  case SOLUTION_TYPE_string:
    printf("string");
    return;
  case SOLUTION_TYPE_unknown:
    printf("unknown");
    return;
  }
};

// TODO: platform dependent "serialization" :( aka just memcpy
char *encode_result_status(const struct result_status *status) {
  char *enc = b64_encode((const unsigned char *)status, sizeof(*status));
  return enc;
}

struct result_status *decode_result_status(char *b64_string) {
  struct result_status *status =
      (struct result_status *)b64_decode(b64_string, strlen(b64_string));
  return status;
}

char *result_status_combine_id(int year, int day, enum AOC_DAY_PART part) {
  char *res;
  asprintf(&res, "solve_%d_%02d_part_%d_%s", year, day, part,
           RESULT_STATUS_VERSION);
  return res;
}

char *result_status_get_id(const struct result_status *status) {
  return result_status_combine_id(status->year, status->day, status->part);
};

#ifdef SQLITE_AVAILABLE
struct db_handle_data {
  sqlite3 *db;
};

struct db_handle_data *db_handle_deref_handle(aoc_manager_handle handle) {
  return (struct db_handle_data *)handle;
}

const char *kv_create_query =
    "CREATE TABLE IF NOT EXISTS kv_store (key "
    "varchar(1023) PRIMARY KEY, value varchar(1023));";

void result_db_initialize_result_status(struct result_status *status) {
  memset(status, 0, sizeof(*status));
  status->year = 2023;
  status->day = -1;
  status->part = AOC_DAY_PART_undefined;
  status->solved = false;
  status->num_solution_lower_bound = 1;
  status->num_solution_upper_bound = LLONG_MAX - 1;
  status->num_solution = -1;
  memset(status->string_solution, 0, AOC_SOL_MAX_LEN + 1);
}

result_db_handle result_db_init_db(char *db_file) {
  struct db_handle_data *handle_data = NULL;
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
  struct db_handle_data *handle_data = db_handle_deref_handle(handle);
  if (handle_data != NULL) {
    printf("close:%d\n", sqlite3_close(handle_data->db));
  }
}

// TODO: raw SQL 🤢
char *get_select_query(const char *key) {
  char *res;
  asprintf(&res, "select * from kv_store where key='%s';", key);
  return res;
}

int get_key_cb(struct result_status **out, int ncols, char **cols,
               char **col_names) {
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
                              enum AOC_DAY_PART part,
                              struct result_status **out) {
  *out = NULL;
  struct db_handle_data *handle_data = db_handle_deref_handle(handle);
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
                               const struct result_status *status,
                               bool overwrite) {
  struct db_handle_data *handle_data = db_handle_deref_handle(handle);
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
  struct result_status status;
  result_db_initialize_result_status(&status);
  status.num_solution_upper_bound = 2823;
  status.num_solution_lower_bound = 112;
  status.day = 1;
  status.part = AOC_DAY_PART_part1;
  // check decoding
  char *encoded = encode_result_status(&status);
  struct result_status *decoded = decode_result_status(encoded);
  assert(memcmp(&status, decoded, sizeof status) == 0);
  free(encoded);
  free(decoded);

  // test insert and select
  char *db_file = "/tmp/testus_teron.db";
  result_db_handle handle = result_db_init_db(db_file);
  result_status_store_entry(handle, &status, true);
  struct result_status *returned_status;
  result_status_load_entry(handle, status.year, status.day, status.part,
                           &returned_status);
  assert(memcmp(&status, returned_status, sizeof status) == 0);
  result_db_close(handle);
  free(returned_status);
}
#endif // SQLITE_AVAILABLE