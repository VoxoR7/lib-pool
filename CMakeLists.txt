cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)

##### lib pool configuration #####

#set(POOL_STATS 100000UL) # print stats every POOL_STATS allocations
#set(DEBUG yes) # uncomment to debug the lib pool

##### lib pool configuration #####

project(pool    LANGUAGES C
                VERSION 0.1.1
                DESCRIPTION "A project of libpool")

add_compile_options(-Wall -Wextra -std=c2x)
if (POOL_STATS)
    add_compile_definitions(POOL_STATS=${POOL_STATS})
endif()
if (DEBUG)
    add_compile_definitions(DEBUG)
    add_compile_options(-g0)
else()
    add_compile_options(-O3)
endif()

add_library(pool)
target_include_directories(pool PUBLIC inc)

add_subdirectory(src)
add_subdirectory(tests)
