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
#ifdef __linux__
#include <unistd.h>
#include <linux/limits.h>
#include <filesystem>
#include <fstream>
#endif

static void glfwErrorCallback(int error, const char* description)
{
	LOG1_WARN("Main", "GLFW ERROR CODE: " + std::to_string(error) + " DESCRIPTION: " + std::string(description));
}

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
#ifdef _WIN32
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

	static void InstallLinuxDesktopFile(const std::string& exePath)
	{
#ifdef __linux__
		namespace fs = std::filesystem;

		auto write_file = [](const fs::path& p, const unsigned char* data, size_t len) {
			fs::create_directories(p.parent_path());
			std::ofstream out(p, std::ios::binary);
			out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(len));
			};

		auto installTo = [&](const fs::path& base)
			{
				// Icons
				write_file(base / "icons/hicolor/16x16/apps/gear.png", Icon16x16, Icon16x16_len);
				write_file(base / "icons/hicolor/32x32/apps/gear.png", Icon32x32, Icon32x32_len);
				write_file(base / "icons/hicolor/48x48/apps/gear.png", Icon48x48, Icon48x48_len);
				// Falls vorhanden:
				// write_file(base / "icons/hicolor/128x128/apps/gear.png", Icon128, Icon128_len);
				// write_file(base / "icons/hicolor/256x256/apps/gear.png", Icon256, Icon256_len);

				// .desktop
				fs::create_directories(base / "applications");
				fs::path desktopFile = base / "applications/gear.desktop";
				std::ofstream desk(desktopFile);
				desk <<
					R"([Desktop Entry]
Version=1.0
Name=Gear
Comment=My cross-platform application
Exec=)" << exePath << R"( %u
TryExec=)" << exePath << R"(
Icon=gear
Terminal=false
Type=Application
Categories=Utility;Development;
)";

				// (optional) Caches aktualisieren – Fehler egal
				std::string baseStr = base.string();
				std::string cmd =
					"update-desktop-database " + (baseStr + "/applications") + " >/dev/null 2>&1 || true; "
					"gtk-update-icon-cache -f " + (baseStr + "/icons/hicolor") + " >/dev/null 2>&1 || true; "
					"xdg-desktop-menu forceupdate >/dev/null 2>&1 || true";
				std::system(cmd.c_str());
			};

		fs::path userBase = fs::path(getenv("HOME")) / ".local/share";
		installTo(userBase);

		fs::path systemBase = "/usr/share";
		if (access(systemBase.c_str(), W_OK) == 0) {
			try { installTo(systemBase); }
			catch (...) {}
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
		glfwSetErrorCallback(glfwErrorCallback);

		if (!glfwInit())
			throw std::runtime_error("Failed to initialize GLFW");

		// ------------------------------------------------------------
		// GLFW / OpenGL Context Initialization (Cross-Platform)
		// ------------------------------------------------------------
#ifdef __APPLE__
	// --- macOS: Core Profile 3.2 + Forward Compatibility ---
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);   // required on macOS
		glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

#elif defined(__linux__)

#ifdef GLFW_WAYLAND_APP_ID
		// Wayland: Set window app_id to match the .desktop filename ("gear.desktop").
		// Without this, the compositor cannot associate the running window with
		// the desktop entry, so no icon will be shown in the taskbar/overview.
		glfwWindowHintString(GLFW_WAYLAND_APP_ID, "gear");
#endif

#if defined(__arm__) || defined(__aarch64__)
		// --- Linux on ARM/ARM64 (e.g. Raspberry Pi): GLES 3.1 ---
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
		// --- Linux on Desktop (x86/x64): Desktop GL 3.3 ---
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

#else
	// --- Windows or other platforms: Desktop GL 3.3 ---
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

		// ------------------------------------------------------------
		// Window Decoration / Frame Style
		// ------------------------------------------------------------
#ifdef _WIN32
	// Custom title bar (we draw our own)
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); // may not work on all GPUs
#else
	// Use system window frame on other platforms
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
#endif

		window = glfwCreateWindow(1280, 720, "GEAR", nullptr, nullptr);
		if (!window)
			throw std::runtime_error("Failed to create GLFW window");

		int fb_w, fb_h;
		glfwGetFramebufferSize(window, &fb_w, &fb_h);

		float xscale, yscale;
		glfwGetWindowContentScale(window, &xscale, &yscale);

		LOG_INFO("GUI Framebuffer: " + std::to_string(fb_w) + "x" + std::to_string(fb_h));
		LOG_INFO("GUI Content scale: " + std::to_string(xscale) + ", " + std::to_string(yscale));

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

#ifdef _WIN32
		SetWindowIcons(window); // only works on Win/Linux, ignored by macOS
#endif

		// Initialize ImGui layer
		guiLayer.Init(window);

#ifdef __APPLE__
		MacSetupMenuAndTitlebar(window);
		MacSyncMenusFromGuiLayer(&guiLayer);
#endif

#ifdef __linux__
		// Get path to binaries
		char exePath[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
		if (len != -1)
		{
			exePath[len] = '\0';
			InstallLinuxDesktopFile(std::string(exePath));
		}
#endif

	}

	void Application::Shutdown()
	{
		guiLayer.Shutdown();

		glfwDestroyWindow(window);
		glfwTerminate();
	}
}
