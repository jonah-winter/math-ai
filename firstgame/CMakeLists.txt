cmake_minimum_required(VERSION 3.25)

project(firstgame LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++23 -g -O3")

add_executable(optai_test
        optai.cpp
)
target_compile_options(optai_test PRIVATE -std=c++23 -O3)
