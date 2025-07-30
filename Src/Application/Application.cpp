#include <GLFW/glfw3.h>
#include <stdexcept>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Application.h"
#include "GUI/GuiLayer.h"
#include "Logger/Logger.h"

namespace cppsandbox
{
    Application::Application()
    {
        Init();
    }

    Application::~Application()
    {
        Shutdown();
    }

    void Application::Init()
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _WIN32
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(1280, 720, "CppSandbox", nullptr, nullptr);
        if (!window)
            throw std::runtime_error("Failed to create GLFW window");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable VSync

        // GUI layer setup
        guiLayer.Init(window);
    }

    void Application::Shutdown()
    {
        guiLayer.Shutdown();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Application::Run()
    {
        // Log some test entries once before main loop
        LOG_INFO("[Demo] Application started.");
        LOG_INFO("[Demo] User '{}' logged in.", "Alice");
        LOG_DEBUG("[Demo] Loading config file from '{}'.", "config/settings.json");
        LOG_WARN("[Demo] Low memory warning: {} MB remaining.", 128);
        LOG_ERROR("[Demo] Failed to connect to server '{}:{}'", "api.example.com", 443);
        LOG_DEBUG("[Demo] Initializing module: {}", "AudioEngine");
        LOG_INFO("[Demo] FPS counter initialized: {} FPS", 144);
        LOG_WARN("[Demo] Frame took too long: {:.2f} ms", 45.3f);
        LOG_ERROR("[Demo] Exception caught: {}", "std::runtime_error(\"Something broke\")");

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            guiLayer.BeginFrame();
            guiLayer.Render();
            guiLayer.EndFrame(window);
            glfwSwapBuffers(window);
        }
    }
}
