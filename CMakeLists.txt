cmake_minimum_required(VERSION 3.10)

project(BohrAtomSimulation VERSION 1.0)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "-Wall -Wextra -fPIC") # -fPIC required for shared libraries on many platforms
find_package(OpenMP REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(bohr_simulation SHARED ${SOURCES})
target_compile_options(bohr_simulation PRIVATE -fopenmp)

add_executable(bohr_simulation_test src/main.c ${SOURCES})
target_compile_options(bohr_simulation_test PRIVATE -g -fopenmp)
target_link_libraries(bohr_simulation_test ${Python3_LIBRARIES} m OpenMP::OpenMP_C)

set_target_properties(bohr_simulation_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)
