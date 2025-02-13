cmake_minimum_required(VERSION 3.10)

project(BohrAtomSimulation VERSION 1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)

# Compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC -flto -fsanitize=address")
# add optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native -mtune=native")

# add define for preprocessor
add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
add_definitions(-DDB_PATH="${PROJECT_SOURCE_DIR}/db/")
add_definitions(-DIM_GUI_INI_PATH="${PROJECT_SOURCE_DIR}/config/imgui.ini")

# Specify the font file location
set(FONT_AWESOME_PATH "${CMAKE_SOURCE_DIR}/assets/fonts/fontawesome-webfont.ttf")

# Copy the font file to the build directory
# configure_file(
#     ${FONT_AWESOME_PATH} ${CMAKE_BINARY_DIR}/assets/fonts/fontawesome-webfont.ttf
#     COPYONLY
# )

list(APPEND CMAKE_PREFIX_PATH "${CONDA_PREFIX}")

# Find Boost libraries
find_package(Boost 1.83.0 REQUIRED system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found")
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

include_directories(${PROJECT_SOURCE_DIR}/external/stb)

# Add GLFW and OpenGL (dependencies for ImGui)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

set(IMPLOT_DIR ${PROJECT_SOURCE_DIR}/external/implot)
file(GLOB IMPLOT_SOURCES
    ${IMPLOT_DIR}/*.cpp
)
include_directories(${IMPLOT_DIR})

find_package(Matplot++ REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SLEEF REQUIRED sleef)
include_directories(${SLEEF_INCLUDE_DIRS})
link_directories(${SLEEF_LIBRARY_DIRS})
add_definitions(${SLEEF_CFLAGS_OTHER})

## add sqlite
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

# Collect source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE C_SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")
add_library(bohr_simulation SHARED ${C_SOURCES})
target_compile_options(bohr_simulation PRIVATE)
target_link_libraries(bohr_simulation PRIVATE)

# Main C++ executable
add_executable(bohr_main src/main.cpp ${C_SOURCES} ${CPP_SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${IMPLOT_SOURCES})
target_compile_options(bohr_main PRIVATE) # -g)
target_link_libraries(bohr_main PRIVATE bohr_simulation glfw OpenGL::GL Matplot++::matplot
 ${Boost_LIBRARIES} ${IMPLOT_HEADERS} ${SLEEF_LIBRARIES} ${SQLite3_LIBRARIES})

# Set output directories
set_target_properties(bohr_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)
