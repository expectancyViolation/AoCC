# // clang-format off
cmake_minimum_required(VERSION 3.16)
project(aocc C)

if (NOT WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif ()
set(CMAKE_C_STANDARD 11)


find_package(CURL)

if (CURL_FOUND)
    add_compile_definitions(CURL_AVAILABLE)
endif ()

find_package(SQLite3)

if (SQLite3_FOUND)
    add_compile_definitions(SQLITE_AVAILABLE)
    #add_compile_definitions(DB_USE_SQLITE)
else ()
    #add_compile_definitions(DB_USE_IN_MEMORY)
endif ()

add_compile_definitions(DB_USE_IN_MEMORY)

if (WIN32)
    add_compile_definitions(_AMD64_)
endif ()


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
        res/hashmap.c/hashmap.c
        src/main.c
        src/util/helpers.c
        src/util/timer.c
        src/util/result_db_sqlite.c
        src/util/aoc_solution_manager.c
        src/util/ll_tuple.c
        src/util/aoc_parse.c
        src/util/parallelize.c
        src/util/aoc.c
        src/util/fenwick.c
        src/2022/year22.c
        src/2022/day01.c
        src/2022/day02.c
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
        src/2023/day12.c
        src/2023/day13.c
        src/2023/day14.c
        src/2023/day15.c
        src/2023/day16.c
        src/2023/day17.c
        src/2023/day18.c
        src/2023/day19.c
        src/2023/day20.c
        src/2023/day21.c
        src/2023/day22.c
        src/2023/day23.c
        src/2023/year23.c
        src/util/aoc_types.c
        src/util/aoc_solving.c
        src/util/result_status.c
        src/util/result_db_in_memory.c
)

if (NOT WIN32)
    target_link_libraries(aocc m)
endif ()
if (CURL_FOUND)
    target_link_libraries(aocc CURL::libcurl)
endif ()
if (SQLite3_FOUND)
    target_link_libraries(aocc SQLite::SQLite3)
endif ()

target_link_libraries(aocc leptonica)
