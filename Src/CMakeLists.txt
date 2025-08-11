# Src/CMakeLists.txt

# ImGui source files (main + backends)
file(GLOB IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Source files excluding main.cpp
set(CPP_GEAR_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/Logger.cpp
    ${IMGUI_SOURCES}
)

set(CPP_GEAR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiLayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiMath.h
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

# Core library target
add_library(GearLib STATIC
    ${CPP_GEAR_SRC}
    ${CPP_GEAR_HEADERS}
)

target_include_directories(GearLib PUBLIC
    ${CMAKE_SOURCE_DIR}/Src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}
)

target_link_libraries(GearLib PRIVATE
    glfw
    ${OPENGL_LIBS}
    fmt::fmt
    IconsEmbedded
)

add_dependencies(GearLib generate_icons)

if (MSVC)
    enable_language(RC)
endif()

# Main application executable
set(APP_ICON_RESOURCE "")
if (MSVC)
    set(APP_ICON_RESOURCE "${CMAKE_SOURCE_DIR}/Assets/Gear.rc")
endif()

add_executable(Gear
    ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
    ${APP_ICON_RESOURCE}
)

# Link against GearLib and IconsEmbedded to get generated headers
target_link_libraries(Gear PRIVATE GearLib)

# Compile options
if(MSVC)
    target_compile_options(Gear PRIVATE
        $<$<CONFIG:Debug>:/Zi /Od>       # Debug Flags MSVC
        $<$<CONFIG:Release>:/O2 /Ot>     # Release Flags MSVC
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(Gear PRIVATE
        $<$<CONFIG:Debug>:-g -O0>       # Debug Flags clang/gcc
        $<$<CONFIG:Release>:-O3>        # Release Flags clang/gcc
    )
endif()

# Windows: disable console window
if (MSVC)
    set_target_properties(Gear PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()
