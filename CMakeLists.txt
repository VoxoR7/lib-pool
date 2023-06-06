cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)

project(pool    LANGUAGES C
                VERSION 0.1.0
                DESCRIPTION "A project of libpool")

add_compile_options(-Wall -Wextra -g3 -std=c17 -DPOOL_STATS)

add_library(pool)
target_include_directories(pool PUBLIC inc)

add_subdirectory(src)
add_subdirectory(tests)
