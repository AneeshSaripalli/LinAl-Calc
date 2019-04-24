cmake_minimum_required(VERSION 3.13)
project(LinAl_Calc)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(src)

add_executable(LinAl_Calc
        include/MathHelpers.h
        include/Matrix.h
        include/Object.h
        include/Runtime.h
        include/TextualHelpers.h
        include/Value.h
        include/Vector.h
        src/Matrix.cpp
        src/Object.cpp
        src/Runtime.cpp
        src/Value.cpp
        src/Vector.cpp
        main.cpp)
