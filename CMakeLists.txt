cmake_minimum_required(VERSION 3.30)
project(comp_eng_culminating)

set(CMAKE_CXX_STANDARD 26)

add_executable(comp_eng_culminating main.cpp
        library.hpp
        util.hpp
        library.cpp)