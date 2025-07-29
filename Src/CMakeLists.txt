# Collect all source files recursively from Src/
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# ImGui source files (main + backends)
file(GLOB IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(CppSandbox
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
)

# Include directories
target_include_directories(CppSandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/Src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(CppSandbox PRIVATE
    glfw
    OpenGL32  # Windows OpenGL lib
)

# Windows: disable console window
if (MSVC)
    set_target_properties(CppSandbox PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()
