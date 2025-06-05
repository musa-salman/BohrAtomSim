get_filename_component(PROJECT_ROOT "${CMAKE_SOURCE_DIR}" ABSOLUTE)

# System/Package Manager Libraries
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

find_package(Boost 1.83.0 REQUIRED COMPONENTS system)
find_package(HDF5 REQUIRED COMPONENTS CXX)
find_package(Matplot++ REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SLEEF REQUIRED sleef)
find_package(SQLite3 REQUIRED)

# SQLiteCpp
CPMAddPackage(
  NAME SQLiteCpp
  GITHUB_REPOSITORY SRombauts/SQLiteCpp
  GIT_TAG 3.3.1
  OPTIONS
    "SQLITECPP_RUN_CPPLINT OFF"
    "SQLITECPP_BUILD_EXAMPLES OFF"
    "SQLITECPP_BUILD_SQLITE3 OFF"
  GIT_SHALLOW TRUE
)

# ImGui
CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  GIT_TAG v1.91.9b-docking
  DOWNLOAD_ONLY TRUE
  GIT_SHALLOW TRUE
)

add_library(imgui_wrapper STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_wrapper PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

# ImPlot
CPMAddPackage(
  NAME implot
  GITHUB_REPOSITORY epezent/implot
  GIT_TAG v0.16
  DOWNLOAD_ONLY TRUE
  GIT_SHALLOW TRUE
)

add_library(implot_wrapper STATIC
  ${implot_SOURCE_DIR}/implot.cpp
  ${implot_SOURCE_DIR}/implot_items.cpp
  ${implot_SOURCE_DIR}/implot_demo.cpp
)

target_include_directories(implot_wrapper PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot_wrapper PUBLIC imgui_wrapper)

# ImGuiFileDialog
CPMAddPackage(
  NAME ImGuiFileDialog
  GITHUB_REPOSITORY aiekick/ImGuiFileDialog
  GIT_TAG v0.6.7
  DOWNLOAD_ONLY TRUE
  GIT_SHALLOW TRUE
)

add_library(imguifiledialog STATIC
  ${ImGuiFileDialog_SOURCE_DIR}/ImGuiFileDialog.cpp
)

target_include_directories(imguifiledialog PUBLIC
  ${ImGuiFileDialog_SOURCE_DIR}
  ${imgui_SOURCE_DIR} # required for <imgui.h>
)

target_link_libraries(imguifiledialog PUBLIC imgui_wrapper)

# nlohmann_json
CPMAddPackage(
  NAME nlohmann_json
  GITHUB_REPOSITORY nlohmann/json
  GIT_TAG v3.12.0
  GIT_SHALLOW TRUE
)

# exprtk
CPMAddPackage(
  NAME exprtk
  GITHUB_REPOSITORY ArashPartow/exprtk
  GIT_TAG master
  GIT_SHALLOW TRUE
)

# magic_enum
CPMAddPackage(
  NAME magic_enum
  GITHUB_REPOSITORY Neargye/magic_enum
  GIT_TAG v0.9.7
  GIT_SHALLOW TRUE
)

# Font Awesome Headers
set(AWESOME_DIR ${PROJECT_ROOT}/external/icons)
file(GLOB AWESOME_HEADERS ${AWESOME_DIR}/*.h ${AWESOME_DIR}/*.hpp)
