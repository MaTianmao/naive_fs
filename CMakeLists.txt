cmake_minimum_required(VERSION 2.8)
project(FileSystem)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g")

include_directories(include/)
add_library(fs include/block.h src/block.c include/file.h src/file.c
            include/inode.h src/inode.c
            include/filename.h src/filename.c include/pathname.h src/pathname.c 
            include/absolute_pathname.h src/absolute_pathname.c
            include/symbolic_link.h src/symbolic_link.c)

set(TEST test/test.c)

add_executable(mtest ${TEST})

target_link_libraries(mtest fs)
