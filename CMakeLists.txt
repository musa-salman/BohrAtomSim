cmake_minimum_required(VERSION 3.10)

project(BohrAtomSimulation VERSION 1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror")

find_package(Python3 COMPONENTS Development REQUIRED)

include_directories(${Python3_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")


add_executable(bohr_simulation ${SOURCES})

target_link_libraries(bohr_simulation ${Python3_LIBRARIES} m)
