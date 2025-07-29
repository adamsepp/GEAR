# CppSandbox

A modern modular C++ application using Dear ImGui and GLFW for GUI rendering — no console, just clean UI.

## ✨ Features

- 🧩 Modular architecture (`Application/`, `Logger/`, `GUI/`, `Tests/`)
- 🖼️ Pure GUI app using Dear ImGui + GLFW + OpenGL
- 📦 All dependencies fetched via `FetchContent` (no manual setup)
- ⚙️ Built with modern **CMake**, supports **Windows**, **Linux**, and **macOS**
- 🧪 Ready for Google Test integration (basic test runner scaffolded)
- ✅ CI with GitHub Actions for cross-platform builds

## 🔧 Build with Visual Studio 2022 (Windows)

1. Open **Visual Studio 2022**
2. File → Open → Folder... → Select the `CppSandbox` root
3. Wait for automatic CMake configuration
4. Open **CMake Targets** window (View → Other Windows → CMake Targets)
5. Right-click `CppSandbox` → **Set as Startup Item**
6. Press **Ctrl+F5** to run (or **F5** to debug)

> Visual Studio uses **CMake + Ninja** under the hood.

## 🛠️ Build on Linux/macOS

Requires: `cmake`, `g++/clang`, `OpenGL` and `X11` (on Linux)

```bash
git clone https://github.com/YOUR_USERNAME/CppSandbox.git
cd CppSandbox
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./CppSandbox
```

> Note: This project uses GLFW + OpenGL. Make sure OpenGL headers and dev packages are available (e.g. `libgl1-mesa-dev` on Linux).

## 🧪 Tests

A simple test runner (`Tests`) is scaffolded. GoogleTest integration is planned.

At the moment, it only prints a placeholder message when executed.

## 📦 Dependencies

- [GLFW](https://github.com/glfw/glfw) – OpenGL context + windowing
- [Dear ImGui](https://github.com/ocornut/imgui) – Immediate-mode GUI

All dependencies are managed with CMake's `FetchContent`.

## 📄 License

MIT License – see `LICENSE` for details.
