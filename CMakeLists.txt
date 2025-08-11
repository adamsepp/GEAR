# CMakeLists.txt

cmake_minimum_required(VERSION 3.20)
project(GEAR VERSION 0.1 LANGUAGES CXX)

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
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)

# fmt (C++ formatting library)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

# stb_image (header-only)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

# Google Test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

# Fetch all declared content
FetchContent_MakeAvailable(glfw imgui fmt stb googletest)

# -------------------------------
# Platform-specific OpenGL linking
# -------------------------------
if (APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    set(OPENGL_LIBS ${OpenGL_LIBRARY})
elseif (WIN32)
    set(OPENGL_LIBS opengl32)
else()
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBS OpenGL::GL)
endif()

# -------------------------------
# Icon Embedding (pure CMake, no external tools)
# -------------------------------

# Source PNGs
set(APP_ICON_PNGS
    ${CMAKE_SOURCE_DIR}/Assets/Icon16x16.png
    ${CMAKE_SOURCE_DIR}/Assets/Icon32x32.png
    ${CMAKE_SOURCE_DIR}/Assets/Icon48x48.png
)

# Where to put the generated headers (inside the build tree)
set(GENERATED_ICON_DIR ${CMAKE_BINARY_DIR}/Assets/IconsGenerated)
file(MAKE_DIRECTORY ${GENERATED_ICON_DIR})

# Helper script that converts any file into a C/C++ header
set(EMBED_SCRIPT ${CMAKE_SOURCE_DIR}/Assets/EmbedFileAsHeader.cmake)

set(GENERATED_ICON_HEADERS "")
foreach(png_file ${APP_ICON_PNGS})
    get_filename_component(fname_noext ${png_file} NAME_WE) # e.g., Icon16x16
    set(header_path ${GENERATED_ICON_DIR}/${fname_noext}.h)

    add_custom_command(
        OUTPUT ${header_path}
        COMMAND ${CMAKE_COMMAND}
            -DINPUT=${png_file}
            -DOUTPUT=${header_path}
            -DSYMBOL=${fname_noext}
            -P ${EMBED_SCRIPT}
        DEPENDS ${png_file} ${EMBED_SCRIPT}
        COMMENT "Embedding ${fname_noext}.png -> ${fname_noext}.h"
        VERBATIM
    )
    list(APPEND GENERATED_ICON_HEADERS ${header_path})
endforeach()

# Build step to generate all headers
add_custom_target(generate_icons ALL DEPENDS ${GENERATED_ICON_HEADERS})

# Interface lib: exports the include path and triggers generation
add_library(IconsEmbedded INTERFACE)
target_include_directories(IconsEmbedded INTERFACE ${GENERATED_ICON_DIR})
add_dependencies(IconsEmbedded generate_icons)

# -------------------------------
# Project subdirectories
# -------------------------------
add_subdirectory(Src)

# -------------------------------
# Optional: Unit tests
# -------------------------------
option(BUILD_TESTS "Build unit tests" ON)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()

# -------------------------------
# Link stdc++fs only on Linux (not macOS or Windows), for compatibility with GCC < 9
# -------------------------------
if (UNIX AND NOT APPLE)
    target_link_libraries(GearLib PRIVATE stdc++fs)
    if (TARGET GearTests)
        target_link_libraries(GearTests PRIVATE stdc++fs)
    endif()
endif()

# -------------------------------
# Developer convenience: Show README.md in VS
# -------------------------------
set(README_PATH "${CMAKE_SOURCE_DIR}/README.md")

if (MSVC)
    # Add it to the project as a virtual file
    source_group("Readme" FILES ${README_PATH})
    set_property(SOURCE ${README_PATH} PROPERTY VS_TOOL_OVERRIDE "Text")

    # Optional: show README in a dedicated target
    add_custom_target(ReadmeDisplay ALL SOURCES ${README_PATH})
endif()
