# 🧭 GEAR Engine -- ImGui + Custom Titlebar Architecture

## Overview

This document describes how **the main application window** integrates
with **Dear ImGui**\
to provide a **custom titlebar**, **transparent background**, and
**native-feeling window controls**\
(minimize, maximize, close, resize, drag).

At this stage, only the **main GLFW window** uses the system.\
Support for additional ImGui viewport windows will be added later.

------------------------------------------------------------------------

## 1️⃣ Rendering Flow (Main Window)

``` mermaid
flowchart TD
    A[GLFW Main Loop] --> B[GuiLayer::BeginFrame]
    B --> C[ImGui::NewFrame]
    C --> D[GuiLayer::Render]
    D --> E[GuiLayer::RenderCustomTitleBar]
    E --> F[Custom Window Content]
    F --> G[ImGui::Render + SwapBuffers]
```

------------------------------------------------------------------------

## 2️⃣ Core Responsibilities

  -----------------------------------------------------------------------
  Component                      Responsibility
  ------------------------------ ----------------------------------------
  **GuiLayer**                   Manages ImGui initialization, frame
                                 lifecycle, and titlebar rendering

  **RenderCustomTitleBar()**     Draws the titlebar, system buttons, and
                                 handles user interactions
                                 (move/resize/maximize/close)

  **WindowRegistry**             Tracks pending window operations (move,
                                 resize, restore) and applies them before
                                 `ImGui::NewFrame()`

  **ImGui / GLFW**               Provide input, context management, and
                                 actual OS window behavior
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 3️⃣ WindowRegistry (New System)

All window movement and resizing operations are now handled through the\
`WindowRegistry` instead of per-frame flags.

This ensures: - consistent timing (applied **before**
`ImGui::NewFrame()`) - no flickering during drag/resize - a clear
separation between **UI logic** (ImGui) and **window control logic**
(GLFW)

------------------------------------------------------------------------

## 4️⃣ GuiLayer Responsibilities

### `GuiLayer::BeginFrame(GLFWwindow* window)`

-   Applies all pending operations from `WindowRegistry`
-   Clears the framebuffer (fully transparent background)
-   Begins a new ImGui frame

### `GuiLayer::RenderCustomTitleBar(GLFWwindow* window)`

-   Draws the logo, menu bar, and window control buttons
-   Detects drag and resize interactions
-   Updates the corresponding `WindowState` in the registry

### `GuiLayer::EndFrame()`

-   Submits the frame and swaps buffers

------------------------------------------------------------------------

## 5️⃣ WindowState Lifecycle

``` mermaid
sequenceDiagram
    participant User
    participant ImGui
    participant GuiLayer
    participant WindowRegistry
    participant GLFW

    User->>ImGui: Click + Drag/Resize
    ImGui->>GuiLayer: Event detected
    GuiLayer->>WindowRegistry: Mark shouldMove / shouldResize
    WindowRegistry->>GLFW: Apply before next frame
```

------------------------------------------------------------------------

## 6️⃣ Future Work

-   Integrate `WindowRegistry` with ImGui Viewports (multi-window)
-   Shared `RenderCustomTitleBar` for all platform windows
-   Fancy Zones & snapping support for secondary windows
-   macOS and Linux-specific titlebar implementations

------------------------------------------------------------------------

## Summary

-   The **WindowRegistry** centralizes all native window operations.\
-   The **GuiLayer** now manages only ImGui-related state and
    rendering.\
-   The system is stable and ready to be extended for ImGui Viewports.
