# CppSandbox

A modern C++ sandbox application with ImGui, GLFW, and modular architecture.

## ✨ Features

- GUI-only C++ application (no console)
- GLFW + OpenGL context
- Dear ImGui GUI loop
- Modular structure (`Application/`, `Logger/`, `GUI/`, `Tests/`)
- Built with CMake (cross-platform)

## 🔧 Building the project with Visual Studio 2022

1. Open **Visual Studio 2022**
2. Go to **File → Open → Folder...**
3. Select the root folder of this repository (`CppSandbox/`)
4. Wait for CMake to configure automatically
5. Open the **CMake Targets** view (if not visible: `View → Other Windows → CMake Targets`)
6. Right-click the target `CppSandbox` → **Set as Startup Item**
7. Press **Ctrl+F5** to run (or **F5** to debug)

> 📝 This project uses CMake + Ninja by default (via Visual Studio's built-in support).

## 🛠️ Building on Linux/macOS (optional)

This project is cross-platform and can be built using CMake on:

- ✅ Windows (Visual Studio 2022)
- ✅ Linux (GCC or Clang)
- ✅ macOS (Clang + Xcode)

```bash
# Example for Linux/macOS
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./CppSandbox
```

## 🗂️ Structure

```
CppSandbox/
├── Src/
│   ├── Main.cpp
│   ├── Application/
│   ├── Logger/
│   ├── GUI/
├── Tests/
├── External/
├── .github/workflows/ci.yml
├── CMakeLists.txt
```

## 📦 Dependencies

- [GLFW](https://github.com/glfw/glfw)
- [Dear ImGui](https://github.com/ocornut/imgui)
- (optional) [ImPlot](https://github.com/epezent/implot) — for future graphing features

All dependencies are pulled via `FetchContent` — no manual installation required.

## 📝 License

This project is licensed under the MIT License. See `LICENSE` file for details.
