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
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiIconListViewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/Logger.cpp
    ${IMGUI_SOURCES}
)

set(CPP_GEAR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiLayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiIconListViewer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/GuiMath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/Logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/LogMessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/LogToFile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/CircularLogBuffer.h
)

# ---- Platform-specific sources (added) ----
set(PLATFORM_SRC "")
set(PLATFORM_HEADERS "")

if (WIN32)
    list(APPEND PLATFORM_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows/WinBorderless.cpp
    )
    list(APPEND PLATFORM_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows/WinBorderless.h
    )
elseif(APPLE)
    list(APPEND PLATFORM_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/MacTitlebar.mm
    )
    list(APPEND PLATFORM_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/MacTitlebar.h
    )
    # Objective-C ARC for our .mm file
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/MacTitlebar.mm
        PROPERTIES COMPILE_FLAGS "-fobjc-arc"
    )
endif()

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
    ${PLATFORM_SRC}
    ${PLATFORM_HEADERS}
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
    EmbeddedFonts
)

# Ensure that icone are generated before compiling GearLib
add_dependencies(GearLib generate_icons)

# Add generated headers to GearLib (so IDEs like VS/CLion can see them)
target_sources(GearLib PRIVATE ${EMBEDDED_FONT_HEADERS})

# Ensure that fonts are generated before compiling GearLib (only on Linux/macOS)
if(UNIX)
    add_dependencies(GearLib generate_fonts)
endif()

# Platform-specific linking/defs for GearLib
if (WIN32)
    target_link_libraries(GearLib PRIVATE dwmapi)
    target_compile_definitions(GearLib PRIVATE GLFW_EXPOSE_NATIVE_WIN32)
elseif(APPLE)
    # For Cocoa-Handle (glfwGetCocoaWindow) and macOS UI
    target_compile_definitions(GearLib PRIVATE GLFW_EXPOSE_NATIVE_COCOA GL_SILENCE_DEPRECATION)
    target_link_libraries(GearLib PRIVATE "-framework Cocoa")
endif()

# MSVC: activate Resource-Compiler, if needed
if (MSVC)
    enable_language(RC)
endif()

# Main application executable
set(APP_ICON_RESOURCE "")

# Main application executable
set(APP_ICON_RESOURCE "")

if (MSVC)
    # On Windows, embed the .ico file via resource script (.rc)
    set(APP_ICON_RESOURCE "${CMAKE_SOURCE_DIR}/Assets/Icons/Gear.rc")

    add_executable(Gear WIN32
        ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
        ${APP_ICON_RESOURCE}
    )
elseif(APPLE)
    # On macOS, embed the .icns file into the .app bundle
    set(MACOSX_BUNDLE_ICON_FILE Gear.icns)

    # Tell CMake to put the .icns file into the Resources folder of the .app bundle
    set_source_files_properties(${CMAKE_SOURCE_DIR}/Assets/Icons/Gear.icns
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(Gear MACOSX_BUNDLE
        ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
        ${CMAKE_SOURCE_DIR}/Assets/Icons/Gear.icns
    )
else()
    # On Linux and others, normal executable
    add_executable(Gear
        ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
    )
endif()

# Link against GearLib to get generated headers
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
