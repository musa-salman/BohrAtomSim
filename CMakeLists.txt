cmake_minimum_required(VERSION 3.10)

project(BohrAtomSimulation VERSION 1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_C_FLAGS "-Wall -Wextra -fPIC") # -fPIC required for shared libraries on many platforms
find_package(Python3 COMPONENTS Development REQUIRED)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
    OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(${Python3_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(bohr_simulation SHARED ${SOURCES})

target_link_libraries(bohr_simulation ${Python3_LIBRARIES} m)

set_target_properties(bohr_simulation PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
)

add_executable(bohr_simulation_test src/test.c)

target_link_libraries(bohr_simulation_test bohr_simulation ${Python3_LIBRARIES} m)

set_target_properties(bohr_simulation_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

target_compile_options(bohr_simulation_test PRIVATE -g)
