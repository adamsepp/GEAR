# CppSandbox/Src/CMakeLists.txt

# ImGui source files (main + backends)
file(GLOB IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Define the executable and source structure
add_executable(CppSandbox)

target_sources(CppSandbox
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Application/Application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiLayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Logger/Logger.cpp

        ${IMGUI_SOURCES}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Application/Application.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiLayer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Logger/Logger.h
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
