
<h1 style="display: flex; align-items: center; gap: 10px; margin: 0;">
  <img src="./Assets/Icons/Icon48x48.png" alt="GEAR Icon" style="width: 48px; height: 48px;">
  GEAR – General Execution & Application Runtime
</h1>

<p align="left">
  <img src="./Docs/Images/Demo.gif" alt="GEAR Demo" width="600">
</p>

**GEAR** is a modular C\+\+20 application framework that uses [Dear ImGui](https://github.com/ocornut/imgui),
[ImPlot](https://github.com/epezent/implot) and [GLFW](https://github.com/glfw/glfw) to provide a minimal,
UI-driven interface without relying on the console.  
It’s designed as a sandbox and runtime core for experimenting with modern C++ features, GUI rendering,
real-time dashboards, and logging.

---

## Features

* Modular structure: `Application/`, `Logger/`, `GUI/`, `Tests/`
* GUI-only interface using Dear ImGui, GLFW, and OpenGL
* Live data visualization with [ImPlot](https://github.com/epezent/implot) (2D/3D plots)
* Custom application icons for executable, GLFW window, and custom title bar
* All dependencies managed via CMake `FetchContent` (no manual downloads)
* Cross-platform build support via CMake (Windows, Linux, macOS)
* Basic GoogleTest setup included
* GitHub Actions configured for CI builds

---

## Development

Build and debug instructions for all platforms are documented in  
👉 [Docs/DEVELOPMENT.md](./Docs/DEVELOPMENT.md)

---

## Tests

Unit tests use [GoogleTest](https://github.com/google/googletest) and run in GitHub Actions on all platforms!  
👉 Details: [Docs/TESTING.md](./Docs/TESTING.md)

---

## Documentation

Detailed documentation about the logging system, its architecture, and performance considerations can be found in  
[Logger.md](./Src/Logger/Logger.md).

---

## Dependencies

This project uses the following main dependencies, all automatically managed via CMake's `FetchContent`:

* [GLFW](https://github.com/glfw/glfw) — window and input management  
* [Dear ImGui](https://github.com/ocornut/imgui) — immediate mode GUI  
* [ImPlot](https://github.com/epezent/implot) — plotting and real-time visualization  
* [fmt](https://github.com/fmtlib/fmt) — modern formatting library  
* [stb_image](https://github.com/nothings/stb) — image loading for window and title bar icons  
* [GoogleTest](https://github.com/google/googletest) — testing framework  

Additionally, a modern C++ compiler (supporting C++20) and CMake (>= 3.20) are required to build the project.

---

## License

This project is licensed under the [MIT License](./LICENSE).
