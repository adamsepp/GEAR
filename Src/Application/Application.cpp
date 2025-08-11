#include <GLFW/glfw3.h>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Generated headers from ${build}/Assets/IconsGenerated
#include "Icon16x16.h"
#include "Icon32x32.h"
#include "Icon48x48.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Application.h"
#include "GUI/GuiLayer.h"
#include "Logger/Logger.h"

namespace // internal linkage
{
	GLFWimage MakeImage(const unsigned char* data, int len)
	{
		int w, h, comp;
		unsigned char* rgba = stbi_load_from_memory(data, len, &w, &h, &comp, 4);
		GLFWimage im{};
		if (rgba) { im.width = w; im.height = h; im.pixels = rgba; }
		return im;
	}

	void SetWindowIcons(GLFWwindow* window)
	{
#ifndef __APPLE__
		std::vector<GLFWimage> icons;
		auto i16 = MakeImage(Icon16x16, Icon16x16_len);
		auto i32 = MakeImage(Icon32x32, Icon32x32_len);
		auto i48 = MakeImage(Icon48x48, Icon48x48_len);

		if (i16.pixels) icons.push_back(i16);
		if (i32.pixels) icons.push_back(i32);
		if (i48.pixels) icons.push_back(i48);

		if (!icons.empty())
		{
			glfwSetWindowIcon(window, static_cast<int>(icons.size()), icons.data());
			for (auto& im : icons)
				stbi_image_free(im.pixels);
		}
#endif
	}
}

namespace gear
{
	Application::Application()
	{
		Init();
	}

	Application::~Application()
	{
		Shutdown();
	}

	void Application::Run()
	{
		LOG_INFO("Application started.");
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();

			guiLayer.BeginFrame(window);
			guiLayer.Render(window);
			guiLayer.EndFrame(window);

			glfwSwapBuffers(window);
		}
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

		// Hide GLFW title bar, since we draw our own with RenderCustomTitleBar()
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		// Make the framebuffer transparent, since we will draw our own edge-rounded background
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

		window = glfwCreateWindow(1280, 720, "GEAR", nullptr, nullptr);
		if (!window)
			throw std::runtime_error("Failed to create GLFW window");

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable VSync

		// Set Window Icon - (needed, since we do glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);)
		// Works on Windows & many Linux WMs.
		// TODO(Linux): For full desktop integration, also install a .desktop file and PNGs to /usr/share/icons.
		// TODO(macOS): glfwSetWindowIcon is ignored, use .icns in app bundle instead.
		SetWindowIcons(window);

		// GUI layer setup
		guiLayer.Init(window);
	}

	void Application::Shutdown()
	{
		guiLayer.Shutdown();

		glfwDestroyWindow(window);
		glfwTerminate();
	}
}
