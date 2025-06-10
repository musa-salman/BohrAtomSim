cmake_minimum_required(VERSION 3.15)
project(BohrAtomSimulation VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23 CACHE STRING "C++ standard" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "C++ standard required" FORCE)
set(CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS "/usr/bin/clang-scan-deps")

if(NOT DEFINED CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)


include(cmake/CPM.cmake)
include(cmake/Dependencies.cmake)

set(FONT_AWESOME_PATH "${CMAKE_SOURCE_DIR}/assets/fonts/fontawesome-webfont.ttf")

# add_library(eom STATIC
#     src/eom/utils.cpp
#     src/eom/spherical.cpp
#     src/eom/polar.cpp
# )
# target_include_directories(eom PUBLIC ${PROJECT_SOURCE_DIR}/include)
# set_target_properties(eom PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(bohr_main
    src/main.cpp
    ${CPP_SOURCES}
)

set_target_properties(bohr_main PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES

    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${PROJECT_SOURCE_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin/release"
)

target_compile_options(bohr_main BEFORE PRIVATE
  -Wall -Wextra -Wpedantic
  $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
  $<$<CONFIG:Release>:-Og -march=native -mtune=native>
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
    # eom
    imgui_wrapper
    implot_wrapper
    imguifiledialog
    SQLiteCpp
    nlohmann_json::nlohmann_json
    glfw
    OpenGL::GL
    matplot
    magic_enum
    GSL
    ${Boost_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${SLEEF_LIBRARIES}
    ${SQLite3_LIBRARIES}
)
