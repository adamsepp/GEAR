# GEAR – General Execution & Application Runtime

**GEAR** is a modular C\+\+20 application framework that uses [Dear ImGui](https://github.com/ocornut/imgui) and [GLFW](https://github.com/glfw/glfw) to provide a minimal, UI-driven interface without relying on the console.  
It’s designed as a sandbox and runtime core for experimenting with modern C++ features, GUI rendering, real-time dashboards, and logging.

## Features

* Modular structure: `Application/`, `Logger/`, `GUI/`, `Tests/`
* GUI-only interface using Dear ImGui, GLFW, and OpenGL
* All dependencies managed via CMake `FetchContent` (no manual downloads)
* Cross-platform build support via CMake (Windows, Linux, macOS)
* Basic GoogleTest setup included
* GitHub Actions configured for CI builds

---

## Building (Visual Studio 2022 on Windows)

1. Open **Visual Studio 2022**
2. Go to **File → Open → Folder...** and select the project root
3. Wait for CMake to finish configuring
4. Open the **CMake Targets** window
5. Right-click on `Gear` and choose **Set as Startup Item**
6. Press **Ctrl+F5** to run (or **F5** to start with debugger)

> Visual Studio uses CMake and Ninja under the hood.

---

## Building on Linux or macOS

Make sure the following are installed:

- `cmake` (version 3.20 or newer)  
  > On Debian-based systems, install with:  
  > `sudo apt install cmake`
- A modern C++ compiler with C++20 and `std::filesystem` support  
  > On Raspberry Pi OS, `g++-11` is not available by default. Use `clang++-11` instead:  
  > `sudo apt install clang-11`
- OpenGL development libraries
- X11 development libraries (on Linux)  
  > For example:  
  > `sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`

### Example build steps:

```bash
mkdir -p ~/Projects && cd ~/Projects                                          # Create the "Projects" directory in your home folder if it doesn't exist, then navigate into it
git clone https://github.com/adamsepp/GEAR.git Gear                           # Clone the GitHub repository into a new folder called "Gear"
cd Gear                                                                       # Navigate into the newly cloned project directory
mkdir build && cd build                                                       # Create a separate build directory and move into it
cmake .. -DCMAKE_CXX_COMPILER=/usr/bin/clang++-11 -DCMAKE_BUILD_TYPE=Release  # Run CMake with the correct compiler and Release build type
make -j$(nproc)                                                               # Compile the project using all available CPU cores for faster builds
LIBGL_ALWAYS_SOFTWARE=1 ./bin/Gear                                            # Run the compiled application with software rendering (for systems without hardware OpenGL)
```

---

## Tests

Unit tests are located in the `Tests/` directory and cover key components like the Logger.

---

## Documentation

Detailed documentation about the logging system, its architecture, and performance considerations can be found in  
[Logger.md](./Src/Logger/Logger.md).

---

## Dependencies

This project uses the following main dependencies, all automatically managed via CMake's `FetchContent`:

* [GLFW](https://github.com/glfw/glfw) — window and input management  
* [Dear ImGui](https://github.com/ocornut/imgui) — immediate mode GUI  
* [fmt](https://github.com/fmtlib/fmt) — modern formatting library  
* [GoogleTest](https://github.com/google/googletest) — testing framework  

Additionally, a modern C++ compiler (supporting C++20) and CMake (>= 3.20) are required to build the project.

All dependencies are handled automatically via CMake's `FetchContent`.

---

## License

This project is licensed under the [MIT License](./LICENSE).
