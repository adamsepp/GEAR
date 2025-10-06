#include "WindowRegistry.h"

WindowState& WindowRegistry::GetState(GLFWwindow* win)
{
    return registry[win]; // auto-creates if missing
}

void WindowRegistry::Remove(GLFWwindow* win)
{
    registry.erase(win);
}

void WindowRegistry::ApplyAllPendingOps()
{
    for (auto& [glfwWin, state] : registry)
    {
        if (!glfwWin)
            continue;

        if (state.shouldRestore)
        {
            if (glfwGetWindowAttrib(glfwWin, GLFW_MAXIMIZED) == GLFW_TRUE)
                glfwRestoreWindow(glfwWin);
            state.shouldRestore = false;
        }

        if (state.shouldMove)
        {
            glfwSetWindowPos(glfwWin, state.pendingMoveX, state.pendingMoveY);
            state.shouldMove = false;
        }

        if (state.shouldResize)
        {
            glfwSetWindowSize(glfwWin, state.pendingResizeW, state.pendingResizeH);
            state.shouldResize = false;
        }
    }
}
