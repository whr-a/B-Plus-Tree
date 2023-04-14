cmake_minimum_required(VERSION 3.16)
project(code)

set(CMAKE_CXX_STANDARD 14)

add_executable(code
        main.cpp
        database.hpp
        exceptions.hpp
        utility.hpp
        vector.hpp
        )
