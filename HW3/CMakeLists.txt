cmake_minimum_required(VERSION 3.20)
project(linking_task C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


add_library(staticpart STATIC
        fred.c
        john.c
)

target_link_libraries(staticpart m)


add_library(fooshared SHARED
        bill.c
        sam.c
)

target_link_libraries(fooshared m)


add_library(preload SHARED
        preload.c
)

target_link_libraries(preload dl)


add_executable(program
        main.c
        wrap.c
)

target_include_directories(program PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(program
        staticpart
        fooshared
        m
)

target_link_options(program PRIVATE
        "-Wl,--wrap=fred"
        "-Wl,--wrap=john"
)