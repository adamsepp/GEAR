# Src/CMakeLists.txt

# ImGui source files (main + backends)
file(GLOB IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Source files excluding main.cpp
set(CPP_SANDBOX_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/Logger.cpp
    ${IMGUI_SOURCES}
)

set(CPP_SANDBOX_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiLayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/Logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/LogMessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/LogToFile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/CircularLogBuffer.h
)

# Select platform-specific OpenGL library
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    set(OPENGL_LIBS ${OPENGL_LIBRARY})
elseif(WIN32)
    set(OPENGL_LIBS opengl32)
else()
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBS OpenGL::GL)
endif()

# Create library target
add_library(CppSandboxLib STATIC
    ${CPP_SANDBOX_SRC}
    ${CPP_SANDBOX_HEADERS}
)

target_include_directories(CppSandboxLib PUBLIC
    ${CMAKE_SOURCE_DIR}/Src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
)

target_link_libraries(CppSandboxLib PRIVATE
    glfw
    ${OPENGL_LIBS}
    fmt::fmt
)

# Create main application executable
add_executable(CppSandbox
    ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)

target_link_libraries(CppSandbox PRIVATE CppSandboxLib)

# Compile options for executable
target_compile_options(CppSandbox PRIVATE
    $<$<CONFIG:Debug>:/Zi /Od>       # Debug flags for MSVC
    $<$<CONFIG:Release>:/O2 /Ot>     # Release flags for MSVC
)

# Windows: disable console window
if (MSVC)
    set_target_properties(CppSandbox PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()
