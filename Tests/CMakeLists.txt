add_executable(CppSandboxTests
    ${CMAKE_CURRENT_SOURCE_DIR}/LoggerTest.cpp
)

target_include_directories(CppSandboxTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Src
)