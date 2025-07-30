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

Make sure the following are installed: `cmake`, `g++` or `clang`, OpenGL, and X11 (on Linux).

```bash
git clone https://github.com/YOUR_USERNAME/CppSandbox.git
cd CppSandbox
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./CppSandbox
```

> On Linux, you may need additional packages like `libgl1-mesa-dev` or `libx11-dev`.

## Tests

The project includes a basic test runner with GoogleTest integration scaffolded.
Currently, only placeholder tests are defined.

## Dependencies

* [GLFW](https://github.com/glfw/glfw)
* [Dear ImGui](https://github.com/ocornut/imgui)

All dependencies are handled automatically via CMake's `FetchContent`.

## License

MIT License – see the `LICENSE` file for details.
