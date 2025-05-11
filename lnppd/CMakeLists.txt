project(lnppd C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(lnppd ${SOURCES})
