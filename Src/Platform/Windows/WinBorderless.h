#pragma once
#ifdef _WIN32
#include <GLFW/glfw3.h>

// Callback for caption hit-test: return true if (x,y) in client coords is draggable.
using TitleHitTestFn = bool(*)(float x, float y, void* user);

// Install borderless style + custom WndProc so only titlebar drags are handled natively.
void HookBorderlessForGLFW(GLFWwindow* window, TitleHitTestFn hitTest, void* user);
#endif
