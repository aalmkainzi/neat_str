cmake_minimum_required(VERSION 3.21)
project(neat_str C)

set(CMAKE_BUILD_TYPE RELEASE)

message("${CMAKE_CURRENT_SOURCE_DIR}")

IF (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:clatest")
ELSE()
    set(CMAKE_C_STANDARD 23)
ENDIF()

set(CMAKE_C_EXTENSIONS on)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/")
add_library(neat_str STATIC "${CMAKE_CURRENT_SOURCE_DIR}/../src/neat_str.c" "${CMAKE_CURRENT_SOURCE_DIR}/../src/neat_core.c")
