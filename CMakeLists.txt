cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
project(BohrAtomSimulation VERSION 1.0)

include(FetchContent)

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

# Add ImGuiFileDialog
set(IMGUIFILEDIALOG_DIR ${PROJECT_SOURCE_DIR}/external/ImGuiFileDialog)
file(GLOB IMGUIFILEDIALOG_SOURCES
    ${IMGUIFILEDIALOG_DIR}/*.cpp
)
include_directories(${IMGUIFILEDIALOG_DIR})

include_directories(${PROJECT_SOURCE_DIR}/external/stb)

# Add GLFW and OpenGL (dependencies for ImGui)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

set(IMPLOT_DIR ${PROJECT_SOURCE_DIR}/external/implot)
file(GLOB IMPLOT_SOURCES
    ${IMPLOT_DIR}/*.cpp
)
include_directories(${IMPLOT_DIR})

# Header only library
set(EXPRTK_DIR ${PROJECT_SOURCE_DIR}/external/exprtk)
file(GLOB EXPRTK_SOURCES
    ${EXPRTK_DIR}/*.hpp
)
include_directories(${EXPRTK_DIR})

set(JSON_DIR ${PROJECT_SOURCE_DIR}/external/json)
file(GLOB JSON_SOURCES
    ${JSON_DIR}/single_include/*.hpp
)

find_package(Matplot++ REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SLEEF REQUIRED sleef)
include_directories(${SLEEF_INCLUDE_DIRS})
link_directories(${SLEEF_LIBRARY_DIRS})
add_definitions(${SLEEF_CFLAGS_OTHER})

## add sqlite
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG master
)

FetchContent_MakeAvailable(SQLiteCpp)
# Collect source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE C_SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")
list(REMOVE_ITEM CPP_SOURCES "${PROJECT_SOURCE_DIR}/src/main.cpp")

# Create eom library
add_library(eom STATIC
    src/eom/utils.cpp
    src/eom/spherical.cpp
    src/eom/polar.cpp
)

list(REMOVE_ITEM CPP_SOURCES
    ${PROJECT_SOURCE_DIR}/src/eom/utils.cpp
    ${PROJECT_SOURCE_DIR}/src/eom/spherical.cpp
    ${PROJECT_SOURCE_DIR}/src/eom/polar.cpp
)

# Create main executable
add_executable(bohr_main src/main.cpp ${C_SOURCES} ${CPP_SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${IMPLOT_SOURCES} ${IMGUIFILEDIALOG_SOURCES} ${AWESOME_HEADERS})
target_compile_options(bohr_main PRIVATE -g)
target_compile_features(bohr_main PRIVATE cxx_std_23)

# Now link all dependencies
target_link_libraries(bohr_main PRIVATE
    eom
    SQLiteCpp
    glfw
    OpenGL::GL
    Matplot++::matplot
    ${HDF5_LIBRARIES}
    ${Boost_LIBRARIES}
    ${IMPLOT_HEADERS}
    ${SLEEF_LIBRARIES}
    ${SQLite3_LIBRARIES}
)

# Set output directories
set_target_properties(bohr_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)
