cmake_minimum_required(VERSION 3.21)
project(neat_str C)

set(CMAKE_BUILD_TYPE RELEASE)

IF (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:clatest")
ELSE()
    set(CMAKE_C_STANDARD 23)
ENDIF()

set(CMAKE_C_EXTENSIONS on)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../bin/")
add_library(neat_str STATIC "../src/neat_str.c" "../src/neat_core.c")
