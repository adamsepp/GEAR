cmake_minimum_required(VERSION 3.20)
project(CppSandbox VERSION 0.1 LANGUAGES CXX)

# Enable modern C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set binary output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# -------------------------------
# Dependencies via FetchContent
# -------------------------------
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8  # Use a fixed version for reproducibility
)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking  # Use 'docking' branch or specific release tag
)

# Fetch all declared content
FetchContent_MakeAvailable(glfw imgui)

# -------------------------------
# Project subdirectories
# -------------------------------
add_subdirectory(Src)

# -------------------------------
# Optional: Unit tests
# -------------------------------
enable_testing()
add_subdirectory(Tests)
