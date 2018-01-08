cmake_minimum_required(VERSION 3.7)
project(Lin_Al)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        include/Matrix.h
        include/Object.h
        include/Vector.h
        src/Matrix.cpp
        src/Object.cpp
        src/Vector.cpp
        main.cpp)

add_executable(Lin_Al ${SOURCE_FILES})