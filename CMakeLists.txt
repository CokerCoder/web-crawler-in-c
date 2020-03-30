cmake_minimum_required(VERSION 3.10)
project(untitled C)

set(CMAKE_C_STANDARD 99)

FILE(GLOB_RECURSE SOURCE_FILE src/*.c lib/*.c)

add_executable(crawler ${SOURCE_FILE})

