cmake_minimum_required(VERSION 4.2)
project(hw_acos C)

set(CMAKE_C_STANDARD 11)

add_library(staticfoo STATIC
        fred.c
        john.c
)

target_link_libraries(staticfoo m)

add_library(sharedfoo SHARED
        bill.c
        sam.c
)

target_link_libraries(sharedfoo m)

add_executable(hw_acos
        program.c
)

target_link_libraries(hw_acos
        staticfoo
        sharedfoo
        m
)

add_executable(hw_acos_wrap
        program.c
        wrap_static.c
)

target_link_libraries(hw_acos_wrap
        staticfoo
        sharedfoo
        m
)

target_link_options(hw_acos_wrap PRIVATE
        "-Wl,--wrap=fred"
        "-Wl,--wrap=john"
)

add_library(preload SHARED
        preload_shared.c
)