cmake_minimum_required(VERSION 3.16)
project(minikvdb)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-march=native -ffast-math)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")


file(GLOB_RECURSE SRC
        src/log/*.cc
        src/log/*.h
        src/memory/*.cc
        src/memory/*.h
        src/utils/*.cc
        src/utils/*.h)

file(GLOB_RECURSE SRC_TEST
            test/test_skiplist.cc)

add_executable(test_skiplist ${SRC} ${SRC_TEST})
