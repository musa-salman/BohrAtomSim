cmake_minimum_required(VERSION 3.15...3.28)
project(BohrAtomSimulation VERSION 1.0)

# Language, Toolchain, Build
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23 CACHE STRING "C++ standard" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "C++ standard required" FORCE)

if(NOT DEFINED CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)

# Compiler Flags
add_compile_options(-Wall -Wextra -Wpedantic)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address -DDEBUG")

# Dependency Setup
include(cmake/CPM.cmake)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++23" COMPILER_SUPPORTS_CXX23)
if(NOT COMPILER_SUPPORTS_CXX23)
    message(FATAL_ERROR "Compiler does not support C++23.")
endif()

include(cmake/Dependencies.cmake)

# Font Path for ImGui
set(FONT_AWESOME_PATH "${CMAKE_SOURCE_DIR}/assets/fonts/fontawesome-webfont.ttf")

# Internal Static Library
add_library(eom STATIC
    src/eom/utils.cpp
    src/eom/spherical.cpp
    src/eom/polar.cpp
)
target_include_directories(eom PUBLIC ${PROJECT_SOURCE_DIR}/include)
set_target_properties(eom PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)

# Source Collection
file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM CPP_SOURCES
    ${PROJECT_SOURCE_DIR}/src/eom/utils.cpp
    ${PROJECT_SOURCE_DIR}/src/eom/spherical.cpp
    ${PROJECT_SOURCE_DIR}/src/eom/polar.cpp
)

# Main Executable
add_executable(bohr_main
    src/main.cpp
    ${CPP_SOURCES}
)

set_target_properties(bohr_main PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

target_include_directories(bohr_main PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${implot_SOURCE_DIR}
    ${ImGuiFileDialog_SOURCE_DIR}
    ${exprtk_SOURCE_DIR}
    ${HDF5_INCLUDE_DIRS}
    ${SLEEF_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
)

target_compile_definitions(bohr_main PRIVATE
    PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
    DB_PATH="${PROJECT_SOURCE_DIR}/db/"
    IM_GUI_INI_PATH="${PROJECT_SOURCE_DIR}/config/imgui.ini"
    ${SLEEF_CFLAGS_OTHER}
)

target_link_libraries(bohr_main PRIVATE
    eom
    imgui_wrapper
    implot_wrapper
    imguifiledialog
    SQLiteCpp
    nlohmann_json::nlohmann_json
    glfw
    OpenGL::GL
    Matplot++::matplot
    ${Boost_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${SLEEF_LIBRARIES}
    ${SQLite3_LIBRARIES}
)

message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
