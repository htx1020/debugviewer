cmake_minimum_required(VERSION 3.16)
project(debugview)

add_library(data_probe ./code/debug_probe_api.c)
target_include_directories(data_probe INTERFACE ./code)

add_executable(demo ./demo/main.c)
target_link_libraries(demo data_probe rt)
