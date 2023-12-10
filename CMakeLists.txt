# // clang-format off
cmake_minimum_required(VERSION 3.16)
project(aocc C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_STANDARD 11)


find_package(CURL REQUIRED)

find_package(SQLite3 REQUIRED)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

add_executable(aocc src/main.c src/util/helpers.c src/util/timer.c src/util/result_db.c
         res/b64.c/buffer.c res/b64.c/encode.c res/b64.c/decode.c
        src/util/aoc_solution_manager.c
        src/util/ll_tuple.c
        src/util/aoc_parse.c
        src/util/parallelize.c
)

target_link_libraries(aocc m)
target_link_libraries(aocc CURL::libcurl)
target_link_libraries(aocc SQLite::SQLite3)
