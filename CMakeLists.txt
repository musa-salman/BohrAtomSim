cmake_minimum_required(VERSION 3.10)

project(BohrAtomSimulation VERSION 1.0)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "-Wall -Wextra -fPIC") # -fPIC required for shared libraries on many platforms
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

find_package(OpenMP REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")

# Find SLEEF library and headers
find_library(SLEEF_LIB sleef REQUIRED)
find_path(SLEEF_INCLUDE_DIR sleef.h REQUIRED)

# Add include directory
include_directories(${SLEEF_INCLUDE_DIR})

add_library(bohr_simulation SHARED ${SOURCES})
target_compile_options(bohr_simulation PRIVATE -fopenmp)

add_executable(bohr_simulation_test src/main.c ${SOURCES} ${HEADERS})
target_compile_options(bohr_simulation_test PRIVATE -fopenmp ${OpenMP_C_FLAGS})
target_link_libraries(bohr_simulation_test ${Python3_LIBRARIES} m OpenMP::OpenMP_C ${SLEEF_LIB})

set_target_properties(bohr_simulation_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)
