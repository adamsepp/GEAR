# CppSandbox

CppSandbox is a modular C++ application that uses Dear ImGui and GLFW to provide a minimal, UI-driven interface without relying on the console. It’s designed as a sandbox for experimenting with modern C++ features, GUI rendering, and logging.

## Features

* Modular structure: `Application/`, `Logger/`, `GUI/`, `Tests/`
* GUI-only interface using Dear ImGui, GLFW, and OpenGL
* All dependencies managed via `FetchContent` (no manual downloads)
* Cross-platform build support via CMake (Windows, Linux, macOS)
* Basic GoogleTest setup included
* GitHub Actions configured for CI builds

## Building (Visual Studio 2022 on Windows)

1. Open Visual Studio 2022
2. Go to **File → Open → Folder...** and select the project root
3. Wait for CMake to finish configuring
4. Open the **CMake Targets** window
5. Right-click on `CppSandbox` and choose **Set as Startup Item**
6. Press **Ctrl+F5** to run (or **F5** to start with debugger)

Note: Visual Studio uses CMake and Ninja under the hood.

## Building on Linux or macOS

Make sure the following are installed:

- `cmake` (version 3.20 or newer)
- A modern C++ compiler (`g++` or `clang++`) with C++20 support
- OpenGL development libraries
- X11 development libraries (on Linux)

Example build steps:

```bash
git clone https://github.com/YOUR_USERNAME/CppSandbox.git
cd CppSandbox
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./CppSandbox
```

> On Linux, you may need additional packages like `libgl1-mesa-dev`, `libx11-dev`, `libxrandr-dev`, `libxi-dev`, `libxinerama-dev` and `libxcursor-dev`.

## Tests

Unit tests are located in the `Tests/` directory and cover key components like the Logger.

## Documentation

Detailed documentation about the logging system, its architecture, and performance considerations can be found in [Logger.md](./Logger/Logger.md).

## Dependencies

This project uses the following main dependencies, all automatically managed via CMake's `FetchContent`:

* [GLFW](https://github.com/glfw/glfw) — window and input management
* [Dear ImGui](https://github.com/ocornut/imgui) — immediate mode GUI
* [fmt](https://github.com/fmtlib/fmt) — modern formatting library
* [GoogleTest](https://github.com/google/googletest) — testing framework

Additionally, a modern C++ compiler (supporting C++20) and CMake (>= 3.20) are required to build the project.

All dependencies are handled automatically via CMake's `FetchContent`.

## License

This project is licensed under the [MIT License](./LICENSE).  
