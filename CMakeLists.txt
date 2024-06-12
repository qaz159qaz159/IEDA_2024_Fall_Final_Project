# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(IEDA_Final_Project)

# Specify the C standard
# set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS_DEBUG "-g")

# Add the executable
add_executable(IEDA_Final_Project main.c read_input.c place.c utils.c optimization.c)

# Include directories
target_include_directories(IEDA_Final_Project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
