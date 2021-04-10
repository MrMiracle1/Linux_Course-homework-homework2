cmake_minimum_required(VERSION 3.0)
project(list)

set(SRC_LIST list.c my_getopt.c)

add_executable(list ${SRC_LIST})
