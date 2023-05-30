cmake_minimum_required(VERSION 3.16)
project(minikvdb)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-march=native -ffast-math)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")


# gtest
find_package(GTest REQUIRED)
include_directories(${GTest_INCLUDE_DIRS})

include_directories(src)

file(GLOB_RECURSE SRC
        src/log/*.cc
        src/log/*.h
        src/memory/*.cc
        src/memory/*.h
        src/utils/*.cc
        src/utils/*.h)

file(GLOB_RECURSE SRC_TEST
            test/*.cc)

add_executable(minikvdb-unitest ${SRC} ${SRC_TEST})
target_link_libraries(minikvdb-unitest PRIVATE gtest pthread)
