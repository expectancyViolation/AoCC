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

add_executable(aocc
        res/b64.c/buffer.c
        res/b64.c/encode.c
        res/b64.c/decode.c
        res/asprintf.c/asprintf.c
        src/main.c
        src/util/helpers.c
        src/util/timer.c
        src/util/result_db.c
        src/util/aoc_solution_manager.c
        src/util/ll_tuple.c
        src/util/aoc_parse.c
        src/util/parallelize.c
        src/util/aoc.c
        src/util/fenwick.c
        src/2022/year22.c
        src/2022/day01.c
        src/2022/day01.h
        src/2023/day01.c
        src/2023/day02.c
        src/2023/day03.c
        src/2023/day04.c
        src/2023/day05.c
        src/2023/day06.c
        src/2023/day07.c
        src/2023/day08.c
        src/2023/day09.c
        src/2023/day10.c
        src/2023/day11.c
        src/2023/year23.c
        src/util/aoc_types.c
)

target_link_libraries(aocc m)
target_link_libraries(aocc CURL::libcurl)
target_link_libraries(aocc SQLite::SQLite3)
