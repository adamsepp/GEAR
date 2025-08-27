# Development Setup

This document describes how to build and debug **GEAR** on different platforms.  
It complements the main project description in [README.md](../README.md).

---

## Contents
- [Building and Debugging on Windows (Visual Studio 2022)](#building-and-debugging-on-windows-visual-studio-2022)
- [Remote Debugging with Visual Studio 2022](#remote-debugging-with-visual-studio-2022)
  - [macOS Setup](#macos-setup)
  - [Raspberry Pi Setup](#raspberry-pi-setup)
- [Working with Visual Studio Code (planned)](#working-with-visual-studio-code-planned)
- [Local building & debugging on macOS (planned)](#local-building--debugging-on-macos-planned)
- [Local building & debugging on Linux](#local-building--debugging-on-linux)
- [Tips](#tips)

---

## Building and Debugging on Windows (Visual Studio 2022)

Visual Studio 2022 can build and debug **GEAR** natively on Windows.  
No extra configuration in `launch.vs.json` is needed.

### Requirements
- Visual Studio 2022 with **Desktop development with C++** workload  
- CMake (included with VS 2022)

### Build & Run
1. Open the project root in Visual Studio (`File → Open → Folder...`)  
2. Wait for CMake to configure  
3. Switch the target selector to **Local Machine**  
4. Choose between **x64-Debug** or **x64-Release** configuration  
5. Press `Ctrl+F5` to run (or `F5` to run with debugger)  

---

## Remote Debugging with Visual Studio 2022

The file [`launch.vs.json`](../launch.vs.json) contains preconfigured debug setups for **remote debugging** on macOS and Raspberry Pi using **Visual Studio 2022 on Windows**.

Each `remoteMachineName` is either read from an **environment variable** or falls back to a **default**.

Examples for environment variables (on Windows, PowerShell):

```powershell
setx GEAR_REMOTE_MAC "user@MacBook.local"
setx GEAR_REMOTE_RPI "pi@raspberrypi.local"
```

If the variables are not set, the fallback values in `launch.vs.json` are used (e.g. `mac.local` or `pi@raspberrypi.local`).

---

### macOS Setup

#### Requirements
- Install Homebrew:
  ```bash
  brew install llvm cmake ninja
  ```
- Build `lldb-mi` from [lldb-tools/lldb-mi](https://github.com/lldb-tools/lldb-mi) and install to `/usr/local/bin/lldb-mi`.
- Add `/usr/local/bin/lldb-mi` and `/usr/bin/lldb` to  
  *System Settings → Privacy & Security → Developer Tools*.
- In Visual Studio: add `"MacBook.local"` as a Cross Platform Connection  
  (*Tools → Options → Cross Platform → Connection Manager*).

#### Launch configuration
- Uses `lldb` as debugger.  
- Path to `lldb-mi`: `/usr/local/bin/lldb-mi`  
- Remote Machine: `${env:GEAR_REMOTE_MAC:mac.local}`

---

### Raspberry Pi Setup

#### Prerequisites

Raspberry Pi 4 or 5 with Raspberry Pi OS

#### Prepare VNC, SSH and Display

To enable VNC and SSH on the Raspberry Pi:

```bash
sudo raspi-config
```
- Interface Options → SSH → YES  
- Interface Options → VNC → YES  
- Advanced Options → Wayland → Labwc (default compositor if Wayland is used)

Afterwards, connect using **Real VNC Viewer** for the desktop, or **PowerShell SSH** from Windows.

#### Prepare the system

```bash
sudo apt update
sudo apt upgrade -y   # optional, recommended on fresh installations

# Compiler & Build Tools
sudo apt install -y build-essential cmake ninja-build git pkg-config

# Debugging Tools
sudo apt install -y gdb gdbserver rsync

# OpenGL / X11 dependencies (required for GLFW + ImGui)
sudo apt install -y   libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev   libgl1-mesa-dev libgl1-mesa-dri mesa-utils

# Wayland support (only if you really want to use Wayland)
sudo apt install -y   wayland-protocols libwayland-dev libxkbcommon-dev

# xxd for converting fonts into C/C++ headers
sudo apt install -y xxd
```

#### Remote debugging with Visual Studio

Once the Raspberry Pi connection has been set up in Visual Studio:

1. Select the configuration **"Raspi-GCC-Debug"**  
2. Wait until CMake has finished configuring  
3. Set **Gear** as the startup project (if not already selected automatically)  
4. Start debugging → the app should launch on the Raspberry Pi (visible in the VNC session) and you can debug remotely

---

## Working with Visual Studio Code (planned)

This section will describe how to configure **Visual Studio Code** with the CMake Tools extension for building and debugging GEAR on different platforms.

---

## Local building & debugging on macOS (planned)

This section will cover how to build and debug GEAR natively on macOS without a Windows host.  
It will include setup instructions for compilers, CMake, and local debugging with `lldb`.

---

## Local building & debugging on Linux

These steps assume that all prerequisites described in  
[Remote Debugging with Visual Studio 2022 → Raspberry Pi Setup](#raspberry-pi-setup)  
have already been installed (compiler, build tools, OpenGL/X11/Wayland dependencies, etc.).

### Example build steps (on the Raspberry Pi)

```bash
# Create the "Projects" directory in your home folder if it doesn't exist, then navigate into it
mkdir -p ~/Projects && cd ~/Projects

# Clone the GitHub repository into a new folder called "Gear"
git clone https://github.com/adamsepp/GEAR.git Gear

# Navigate into the newly cloned project directory
cd Gear

# Create a separate build directory and move into it
mkdir build && cd build

# Configure the project with CMake in Debug mode
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug

# Compile the project using all available CPU cores
make -j$(nproc)

# Run the compiled application
./bin/Gear
```

---

## Tips

- If you want to hardcode your personal host/user, you can create a local  
  `launch.vs.user.json` and add it to `.gitignore`.  
- This way the repo version remains generic and usable for everyone.
