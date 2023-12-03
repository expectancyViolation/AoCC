cmake_minimum_required(VERSION 3.16)
project(aocc C)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_STANDARD 11)

add_executable(aocc main.c timer.c)
