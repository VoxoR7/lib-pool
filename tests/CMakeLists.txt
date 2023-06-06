add_executable(tests)

target_link_libraries(tests pool)
target_sources(tests PUBLIC main.c)
