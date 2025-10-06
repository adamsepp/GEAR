#pragma once
#include <unordered_map>
#include <GLFW/glfw3.h>

struct WindowState
{
    bool shouldMove = false;
    bool shouldResize = false;
    bool shouldRestore = false;

    int pendingMoveX = 0;
    int pendingMoveY = 0;
    int pendingResizeW = 0;
    int pendingResizeH = 0;

    bool isMaximized = false;
    bool isDragging = false;
};

class WindowRegistry
{
public:
    WindowState& GetState(GLFWwindow* win);
    void Remove(GLFWwindow* win);
    void ApplyAllPendingOps();

private:
    std::unordered_map<GLFWwindow*, WindowState> registry;
};
