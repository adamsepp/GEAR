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

#ifdef _WIN32
#include "Platform/Windows/WinBorderless.h"
#endif
#ifdef __APPLE__
#include "Platform/Mac/MacTitlebar.h"
#endif

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

#ifdef __APPLE__
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _WIN32
		// --- Windows-specific: draw our own custom title bar ---
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // disable default OS frame
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); // background transparency for rounded edges
#else
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE); // keep default decoration on other platforms
#endif

		window = glfwCreateWindow(1280, 720, "GEAR", nullptr, nullptr);
		if (!window)
			throw std::runtime_error("Failed to create GLFW window");

#ifdef _WIN32
		// Hook custom WndProc: lets Windows handle dragging (for FancyZones, Snap, etc.)
		HookBorderlessForGLFW(window,
			[](float x, float y, void* user) -> bool
			{
				auto* gui = static_cast<gear::GuiLayer*>(user);
				// query flag from custom titlebar (set in RenderCustomTitleBar)
				return gui->GetTitleBarAllowDrag() && (y >= 0.0f) && (y <= gui->GetTitleBarHeight() * gui->GetDpiScale());
			},
			&guiLayer);

		// Store 'this' for callbacks that need Application instance
		glfwSetWindowUserPointer(window, this);

		// Handle WM_PAINT/refresh: force one redraw if maximized+restored via OS drag (to show it always correct when dragging starts)
		glfwSetWindowRefreshCallback(window, [](GLFWwindow* w)
			{
				auto* app = static_cast<gear::Application*>(glfwGetWindowUserPointer(w));
				if (!app)
					return;

				if (app->guiLayer.GetIsMaximized())
				{
					// Render one frame so content size matches new restored window size
					glfwMakeContextCurrent(w);
					app->guiLayer.BeginFrame(w);
					app->guiLayer.Render(w);
					app->guiLayer.EndFrame(w);
					glfwSwapBuffers(w);
				}
			});
#endif

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable VSync

#ifndef __APPLE__
		SetWindowIcons(window); // only works on Win/Linux, ignored by macOS
#endif

		// Initialize ImGui layer
		guiLayer.Init(window);

#ifdef __APPLE__
		MacSetupMenuAndTitlebar(window);
		MacSyncMenusFromGuiLayer(&guiLayer);
#endif
	}

	void Application::Shutdown()
	{
		guiLayer.Shutdown();

		glfwDestroyWindow(window);
		glfwTerminate();
	}
}
