# Testing GEAR

This document describes how to run, extend, and understand the tests in **GEAR**.

---

## Overview

- **Framework**: [GoogleTest](https://github.com/google/googletest)  
- **Location**: All test sources are located in the [`Tests/`](../Tests) directory  
- **CI**: Tests are executed automatically in GitHub Actions on Windows, macOS, and Linux (Ubuntu).  
  The Raspberry Pi is supported as a **manual remote debugging target** only and is not part of CI.

---

## Running Tests

### From Visual Studio 2022
1. Open the project in Visual Studio  
2. Open the **CMake Targets** window  
3. Set `Tests` as the startup item  
4. Press `Ctrl+F5` to run or `F5` to debug tests  

### From Command Line
```bash
cd build
ctest --output-on-failure
```

This will build and execute all tests discovered by CMake/GoogleTest.

---

## Writing New Tests

- Tests are located in the `Tests/` folder (example: [`LoggerTest.cpp`](../Tests/LoggerTest.cpp))  
- Use GoogleTest macros like `TEST`, `EXPECT_EQ`, `ASSERT_TRUE`, etc.  
- Add new `.cpp` files under `Tests/` and reference them in [`Tests/CMakeLists.txt`](../Tests/CMakeLists.txt)

### Example
```cpp
#include <gtest/gtest.h>
#include "Logger/Logger.h"

TEST(Logger, WritesMessage) {
    Logger log;
    log.write("Hello");
    EXPECT_TRUE(log.hasMessages());
}
```

---

## Continuous Integration (CI)

Tests run automatically in GitHub Actions on every push and pull request.  
The workflow is defined in [`.github/workflows/ci.yml`](../.github/workflows/ci.yml).  

Currently the matrix runs on:
- `windows-latest`
- `macos-latest`
- `ubuntu-latest`

This ensures cross-platform coverage for all major environments.  
The Raspberry Pi is not included in CI and must be tested manually.

---

## Platform Legend

| Platform        | How it’s tested                                      |
|-----------------|------------------------------------------------------|
| **Windows**     | Native in Visual Studio + CI (`windows-latest`)      |
| **macOS**       | Remote debugging in VS / CI (`macos-latest`)         |
| **Linux (PC)**  | CI (`ubuntu-latest`)                                 |
| **Raspberry Pi**| Manual remote debugging in Visual Studio only (no CI)|

---

## Notes

- All tests run in CI with GitHub Actions on Windows, macOS, and Linux.  
- To see CI results, check the **Actions** tab on the repository.  
- When contributing, please make sure new functionality is covered by tests.
