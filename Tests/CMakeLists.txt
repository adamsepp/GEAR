# Tests/CMakeLists.txt

add_executable(CppSandboxTests
    ${CMAKE_CURRENT_SOURCE_DIR}/LoggerTest.cpp
)

target_include_directories(CppSandboxTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Src
)

target_link_libraries(CppSandboxTests PRIVATE
    CppSandboxLib
)

add_test(NAME LoggerTest COMMAND CppSandboxTests)

if (MSVC)
    set_target_properties(CppSandboxTests PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()
