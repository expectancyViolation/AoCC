#ifdef DB_USE_IN_MEMORY
#include "result_db.h"

typedef struct _DbHandleData {

} DbHandleData;


result_db_handle result_db_init_db(char *db_file){

}

void result_db_close(result_db_handle handle){

}

bool result_status_load_entry(result_db_handle handle, int year, int day,
                              enum AOC_DAY_PART part, ResultStatus **out){

}

bool result_status_store_entry(result_db_handle handle,
                               const ResultStatus *status, bool overwrite){

}

void result_db_test();

#endif //DB_USE_IN_MEMORY
