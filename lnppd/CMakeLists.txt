cmake_minimum_required(VERSION 3.30)

project(lnppd C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE C_SOURCES "src/*.c")

add_executable(lnppd ${C_SOURCES})
