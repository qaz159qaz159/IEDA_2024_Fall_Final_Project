# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(IEDA_Final_Project VERSION 1.0 LANGUAGES CXX)

# Specify the C standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# Add the executable
add_executable(IEDA_Final_Project
        main.cpp
        structure.h
        read_input.cpp
        grid.cpp
        grid.h
        utils.h utils.cpp
        optimization.cpp optimization.h)

# Include directories
target_include_directories(IEDA_Final_Project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
