cmake_minimum_required(VERSION 3.30)

project(lnppd_backend C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE C_SOURCES "src/*.c")

find_package(Cups REQUIRED)

add_executable(lnppd_backend ${C_SOURCES})

target_link_libraries(lnppd_backend PRIVATE ${CUPS_LIBARIES})
target_include_directories(lnppd_backend PRIVATE ${CUPS_INCLUDE_DIRS})
target_link_libraries(lnppd_backend PRIVATE cupsbackend)
