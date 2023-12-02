cmake_minimum_required(VERSION 3.16)
project(aocc C)

set(CMAKE_C_STANDARD 11)

add_executable(aocc day01.h day02.h main.c helper.h)
