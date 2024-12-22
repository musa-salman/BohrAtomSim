cmake_minimum_required(VERSION 3.10)

project(BohrAtomSimulation VERSION 1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")

set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC")

# Specify the font file location
set(FONT_AWESOME_PATH "${CMAKE_SOURCE_DIR}/assets/fonts/fontawesome-webfont.ttf")

# Copy the font file to the build directory
configure_file(
    ${FONT_AWESOME_PATH} ${CMAKE_BINARY_DIR}/assets/fonts/fontawesome-webfont.ttf
    COPYONLY
)

# Find Boost libraries
find_package(Boost 1.83.0 REQUIRED system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found")
endif()

# Find HDF5 library
find_package(HDF5 REQUIRED COMPONENTS CXX)

if(HDF5_FOUND)
    include_directories(${HDF5_INCLUDE_DIRS})
    message(STATUS "HDF5 found: ${HDF5_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "HDF5 not found")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

# Add Awesome Icons
set(AWESOME_DIR ${PROJECT_SOURCE_DIR}/external/icons)
file(GLOB AWESOME_HEADERS ${AWESOME_DIR}/*.h ${AWESOME_DIR}/*.hpp)
include_directories(${AWESOME_DIR})

# Add Dear ImGui
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/external/imgui)
file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

# Add GLFW and OpenGL (dependencies for ImGui)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

set(IMPLOT_DIR ${PROJECT_SOURCE_DIR}/external/implot)
file(GLOB IMPLOT_SOURCES
    ${IMPLOT_DIR}/*.cpp
)
include_directories(${IMPLOT_DIR})

# Collect source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE C_SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")
add_library(bohr_simulation SHARED ${C_SOURCES})
target_compile_options(bohr_simulation PRIVATE)
target_link_libraries(bohr_simulation PRIVATE)

# Main C++ executable
add_executable(bohr_main src/main.cpp ${C_SOURCES} ${CPP_SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${IMPLOT_SOURCES})
target_compile_options(bohr_main PRIVATE -g)
target_link_libraries(bohr_main PRIVATE bohr_simulation glfw OpenGL::GL ${Boost_LIBRARIES} ${HDF5_LIBRARIES} ${IMPLOT_HEADERS})

# Set output directories
set_target_properties(bohr_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)
