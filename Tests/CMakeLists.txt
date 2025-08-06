# Tests/CMakeLists.txt

# Unit test executable
add_executable(GearTests
    ${CMAKE_CURRENT_SOURCE_DIR}/LoggerTest.cpp
)

target_include_directories(GearTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Src
)

target_link_libraries(GearTests PRIVATE
    GearLib
    fmt::fmt
    gtest
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(GearTests)

if (MSVC)
    set_target_properties(GearTests PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()
