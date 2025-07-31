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
		LOG_INFO("Application started.");
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
