#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <functional>
#include <chrono>

#include "GuiLayer.h"
#include "GuiMath.h"
#include "GuiIconListViewer.h"
#include "Logger/Logger.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef _WIN32
#include "Platform/Windows/WinBorderless.h"
#endif

#ifdef __APPLE__
#include "Platform/Mac/MacTitlebar.h"
#endif

// Generated headers from ${build}/Assets/IconsGenerated
#include "Icon32x32.h"

// Fonts
#ifdef _WIN32
#define FONT_INTER_REGULAR   201
#define FONT_INTER_BOLD      202
#define FONT_FA_SOLID        203
#define FONT_FA_REGULAR      204
#define FONT_FA_BRANDS       205
#else
// Generated headers from ${build}/EmbeddedFonts
#include "Inter_Regular.h"
#include "Inter_Bold.h"
#include "Font_Awesome_7_Free_Solid_900.h"
#include "Font_Awesome_7_Free_Regular_400.h"
#include "Font_Awesome_7_Brands_Regular_400.h"
#endif

#include "../Assets/Fonts/IconsFontAwesome7.h"
#include "../Assets/Fonts/IconsFontAwesome7Brands.h"

static ImTextureID logTextureID = (ImTextureID)nullptr;
static ImTextureID LoadLogoTextureFromMemory(const unsigned char* data, int len)
{
	int w, h, comp;
	unsigned char* rgba = stbi_load_from_memory(data, len, &w, &h, &comp, 4);
	if (!rgba)
		return (ImTextureID)nullptr;

	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	stbi_image_free(rgba);

	return (ImTextureID)(intptr_t)texId;
}

// Fonts
static ImFont* fontRegular = nullptr;
static ImFont* fontBold = nullptr;

namespace gear
{
	void GuiLayer::Init(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// --- Fonts ---
		io.Fonts->Clear();

		// Ranges für Font Awesome
		static const ImWchar fontRegularRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		static const ImWchar fontBrandRanges[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 };

		float fontSize = 16.0f;

#ifdef _WIN32
		// Inter Regular (Basis-Font mit Merge für Icons)
		ImFontConfig baseConfig;
		baseConfig.FontDataOwnedByAtlas = false; // resource memory must NOT be freed by ImGui
		{
			ResourceData regData = LoadResourceData(FONT_INTER_REGULAR);
			if (regData.data)
				fontRegular = io.Fonts->AddFontFromMemoryTTF((void*)regData.data, (int)regData.size, fontSize, &baseConfig, io.Fonts->GetGlyphRangesDefault());

			if (fontRegular)
			{
				ImFontConfig cfg;
				cfg.MergeMode = true;
				cfg.PixelSnapH = true;
				cfg.FontDataOwnedByAtlas = false; // resource memory must NOT be freed by ImGui

				// Font Awesome Solid
				ResourceData faSolid = LoadResourceData(FONT_FA_SOLID);
				if (faSolid.data)
					io.Fonts->AddFontFromMemoryTTF((void*)faSolid.data, (int)faSolid.size, fontSize, &cfg, fontRegularRanges);

				// Font Awesome Regular
				ResourceData faRegular = LoadResourceData(FONT_FA_REGULAR);
				if (faRegular.data)
					io.Fonts->AddFontFromMemoryTTF((void*)faRegular.data, (int)faRegular.size, fontSize, &cfg, fontRegularRanges);

				// Font Awesome Brands
				ResourceData faBrands = LoadResourceData(FONT_FA_BRANDS);
				if (faBrands.data)
					io.Fonts->AddFontFromMemoryTTF((void*)faBrands.data, (int)faBrands.size, fontSize, &cfg, fontBrandRanges);
			}
		}

		// Inter Bold (separat, ohne Merge)
		{
			ResourceData boldData = LoadResourceData(FONT_INTER_BOLD);
			if (boldData.data)
				fontBold = io.Fonts->AddFontFromMemoryTTF((void*)boldData.data, (int)boldData.size, fontSize, &baseConfig, io.Fonts->GetGlyphRangesDefault());
		}

#else // Linux / macOS via generated headers
		ImFontConfig baseConfig;
		baseConfig.FontDataOwnedByAtlas = false; // resource memory must NOT be freed by ImGui

		// Inter Regular (base font with merge for icons)
		fontRegular = io.Fonts->AddFontFromMemoryTTF((void*)inter_regular_ttf, (int)inter_regular_ttf_len, fontSize, &baseConfig, io.Fonts->GetGlyphRangesDefault());

		if (fontRegular)
		{
			ImFontConfig cfg;
			cfg.MergeMode = true;
			cfg.PixelSnapH = true;
			cfg.FontDataOwnedByAtlas = false; // resource memory must NOT be freed by ImGui

			// Font Awesome Solid
			io.Fonts->AddFontFromMemoryTTF((void*)font_awesome_7_free_solid_900_otf, (int)font_awesome_7_free_solid_900_otf_len, fontSize, &cfg, fontRegularRanges);

			// Font Awesome Regular
			io.Fonts->AddFontFromMemoryTTF((void*)font_awesome_7_free_regular_400_otf, (int)font_awesome_7_free_regular_400_otf_len, fontSize, &cfg, fontRegularRanges);

			// Font Awesome Brands
			io.Fonts->AddFontFromMemoryTTF((void*)font_awesome_7_brands_regular_400_otf, (int)font_awesome_7_brands_regular_400_otf_len, fontSize, &cfg, fontBrandRanges);
		}

		// Inter Bold (separately, without merge)
		fontBold = io.Fonts->AddFontFromMemoryTTF((void*)inter_bold_ttf, (int)inter_bold_ttf_len, fontSize, &baseConfig, io.Fonts->GetGlyphRangesDefault());
#endif

		// --- Core ImGui config flags ---
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // enable docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // multi-viewport / OS windows

#ifdef __APPLE__
		// macOS-native text navigation and ⌘ shortcuts
		io.ConfigMacOSXBehaviors = true;
		titleBarHeight = 28.0f; // Mac standard
#endif
		// --- Style ---
		ApplyCustomDarkTheme();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// Viewport windows are real OS windows; rounding must be 0 for seamless look
			style.WindowRounding = 0.0f;
			// Ensure alpha=1 for viewport windows, or they may appear translucent
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// --- Backend init (GLFW + OpenGL3) ---
		// Install callbacks = true is fine unless you manage GLFW callbacks yourself.
		ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks=*/true);

		// Select GLSL version *after* context creation
#ifdef __APPLE__
		ImGui_ImplOpenGL3_Init("#version 150");      // GL 3.2 Core
#elif defined(__linux__) && (defined(__arm__) || defined(__aarch64__))
		ImGui_ImplOpenGL3_Init("#version 300 es");   // GLES 3.1
#else
		ImGui_ImplOpenGL3_Init("#version 330 core"); // GL 3.3 Core
#endif

		// Register all default menues
		RegisterDefaultMenus(window);
	}

	void GuiLayer::Shutdown()
	{
		if (logTextureID)
		{
			GLuint tex = (GLuint)(intptr_t)logTextureID;
			glDeleteTextures(1, &tex);
			logTextureID = (ImTextureID)nullptr;
		}
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void GuiLayer::BeginFrame(GLFWwindow* window)
	{
		// ------------------------------------------------------------
		// Clear the framebuffer with a fully transparent background.
		// This is necessary when using a transparent GLFW window
		// (glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE))
		// and drawing your own rounded background via ImGui.
		// ------------------------------------------------------------
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // RGBA: fully transparent
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------------------------------------------------------------
		// Apply pending window move/resize *before* starting the ImGui
		// frame. ImGui reads and caches window state at NewFrame(),
		// so changes after that would cause visual glitches or offset
		// layout during resizing or dragging.
		// ------------------------------------------------------------
		if (shouldRestoreWindow)
		{
			if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE)
				glfwRestoreWindow(window); // break maximized state
			shouldRestoreWindow = false;
		}
		if (shouldMoveWindow)
		{
			glfwSetWindowPos(window, pendingMoveX, pendingMoveY);
			shouldMoveWindow = false;
		}
		if (shouldResizeWindow)
		{
			glfwSetWindowSize(window, pendingResizeW, pendingResizeH);
			shouldResizeWindow = false;
		}

		// ------------------------------------------------------------
		// Begin a new ImGui frame
		// ------------------------------------------------------------
		ImGui_ImplOpenGL3_NewFrame();   // Prepare OpenGL3 bindings
		ImGui_ImplGlfw_NewFrame();      // Prepare GLFW input bindings
		ImGui::NewFrame();              // Begin ImGui frame logic

		// ------------------------------------------------------------
		// Draw a rounded window background behind all ImGui windows.
		// Platform-specific tweaks:
		//   - Linux: no rounding (compositor/transparency uncertain)
		//   - Windows/others: rounded unless maximized
		//   - macOS: always rounded
		// ------------------------------------------------------------
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImDrawList* drawList = ImGui::GetBackgroundDrawList(const_cast<ImGuiViewport*>(viewport));

		ImVec2 pos = viewport->Pos;
		ImVec2 size = viewport->Size;
		ImU32 bgColor = ImGui::GetColorU32(ImGuiCol_WindowBg);

		float rounding = 0.0f;
		ImDrawFlags drawFlags = ImDrawFlags_None;

#if defined(__linux__)
		// Linux: disable rounding (compositor support uncertain)
		rounding = 0.0f;
		drawFlags = ImDrawFlags_None;
#elif defined(__APPLE__)
		// macOS: always rounded
		rounding = 8.0f;
		drawFlags = ImDrawFlags_RoundCornersAll;
#else
		// Windows / others: rounded unless maximized
		bool isMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
		rounding = isMaximized ? 0.0f : 8.0f;
		drawFlags = isMaximized ? ImDrawFlags_None : ImDrawFlags_RoundCornersAll;
#endif

		// Draw filled background
		drawList->AddRectFilled(pos, pos + size, bgColor, rounding, drawFlags);

		// Optional border if rounded
		if (rounding > 0.0f && drawFlags != ImDrawFlags_None)
			drawList->AddRect(pos, pos + size, ImGui::GetColorU32(ImGuiCol_Border), rounding, drawFlags, 1.0f);
	}

	void GuiLayer::Render(GLFWwindow* window)
	{
		RenderCustomTitleBar(window);

		// Setup main docking window (below custom title bar)
		constexpr ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		// Shift dockspace window down below the custom title bar
		ImVec2 dockPos = viewport->WorkPos;
		dockPos.y += titleBarHeight;

		ImVec2 dockSize = viewport->WorkSize;
		dockSize.y -= titleBarHeight;

		ImGui::SetNextWindowPos(dockPos);
		ImGui::SetNextWindowSize(dockSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background, since we use a global rounded background

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3.0f, 3.0f));

		ImGui::Begin("MainDockSpace", nullptr, windowFlags);
		ImGui::PopStyleVar(1);

		// Create the actual dock space
		ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));
		ImGui::End();

		// Initialize docking layout once
		static bool dockInitialized = false;
		if (!dockInitialized)
		{
			dockInitialized = true;

			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);

			// Match the dock space size to the window (adjusted for title bar)
			ImVec2 builderSize = viewport->WorkSize;
			builderSize.y -= titleBarHeight;
			ImGui::DockBuilderSetNodeSize(dockspaceID, builderSize);

			// Split dock area: top and bottom sections
			ImGuiID dockMainID = dockspaceID;
			ImGuiID bottomDockID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.3f, nullptr, &dockMainID);
			ImGuiID topDockID = dockMainID;

			// Assign windows to dock regions
			ImGui::DockBuilderDockWindow("Main", topDockID);
			ImGui::DockBuilderDockWindow("Logger", bottomDockID);
			ImGui::DockBuilderDockWindow("Dear ImGui Demo", topDockID);

			ImGui::DockBuilderFinish(dockspaceID);
		}

		// Render docked windows
		ShowMainWindow();
		ShowLoggerWindow();

		if (showImGuiDemoWindow)
			ImGui::ShowDemoWindow(&showImGuiDemoWindow);

		if (showIconListViewerWindow)
			ShowIconListView(&showIconListViewerWindow);
	}

	void GuiLayer::EndFrame(GLFWwindow* window)
	{
		ImGuiIO& io = ImGui::GetIO();
		// Render main ImGui draw data to current framebuffer
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void GuiLayer::AddMenu(MenuDef menu)
	{
		menus.push_back(std::move(menu));
	}

	void GuiLayer::RegisterDefaultMenus(GLFWwindow* window)
	{
		if (!menus.empty())
			return; // idempotent

		MenuDef fileMenu{ "File", {
			MenuItem{ "Exit", std::nullopt /*std::string("Alt+F4")*/, [=]() { if (window) glfwSetWindowShouldClose(window, GLFW_TRUE); }, false }
		} };

		MenuDef viewMenu{ "View", {
			MenuItem{ "Show ImGui Demo", std::nullopt,       [this]() { showImGuiDemoWindow = true; }, false },
			MenuItem{ "Show Icon List Viewer", std::nullopt, [this]() { showIconListViewerWindow = true; }, false }
		} };

		menus.push_back(std::move(fileMenu));
		menus.push_back(std::move(viewMenu));
	}

	void GuiLayer::ApplyCustomDarkTheme()
	{
		auto& colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	void GuiLayer::DrawTopMenuItem(const char* label, const std::function<void()>& contentFn, bool& isOverSysButton)
	{
		static const char* activeTopMenuName = nullptr;

		float paddingX = ImGui::GetStyle().FramePadding.x * 2;
		ImVec2 labelSize = ImGui::CalcTextSize(label);
		float buttonHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		float buttonWidth = labelSize.x + paddingX * 2.0f;

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 size = ImVec2(buttonWidth, buttonHeight);

		bool hovered = ImGui::IsMouseHoveringRect(pos, pos + size);
		bool isActive = (activeTopMenuName != nullptr && strcmp(activeTopMenuName, label) == 0);

		// Highlight background if the item is hovered or active
		if (hovered || isActive)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(
				pos,
				pos + size,
				IM_COL32(60, 60, 60, 255)
			);
			isOverSysButton = true;
		}

		// Create an invisible button that responds to input
		ImGui::InvisibleButton(label, size);

		// Draw the label manually, centered vertically
		ImGui::SetCursorScreenPos(ImVec2(
			pos.x + paddingX,
			pos.y + (buttonHeight - labelSize.y) * 0.5f
		));
		ImGui::TextUnformatted(label);

		// Click behavior: toggle the popup
		if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (isActive)
				activeTopMenuName = nullptr;
			else
			{
				activeTopMenuName = label;
				ImGui::OpenPopup(label);
			}
			isOverSysButton = true;
		}

		// Hover behavior: switch active menu if another is already open
		if (hovered && activeTopMenuName != nullptr && !isActive)
		{
			activeTopMenuName = label;
			ImGui::OpenPopup(label);
			isOverSysButton = true;
		}

		// Position the popup directly below the menu item
		ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + size.y - 1.0f));

		// Render the popup if it's open
		if (ImGui::BeginPopup(label))
		{
			contentFn();
			ImGui::EndPopup();
			isOverSysButton = true;
		}
		else if (isActive)
		{
			// If the popup was closed externally, clear active state
			activeTopMenuName = nullptr;
		}

		// Ensure the next menu item is placed on the same line
		ImGui::SameLine();
	}

	void GuiLayer::RenderCustomTitleBar(GLFWwindow* window)
	{
#ifdef _WIN32
		// DPI-aware values
		float dpiScale = ImGui::GetIO().DisplayFramebufferScale.y;
		float padding = 8.0f * dpiScale;
		float logoSize = (titleBarHeight - padding / 2) * dpiScale; // Dynamically scale logo with title bar height
		static bool isResizing = false;
		const float resizeBorder = 5.0f * dpiScale; // Distance from window edge within which resizing is triggered

		int winPosX, winPosY;
		glfwGetWindowPos(window, &winPosX, &winPosY);
		int winSizeX, winSizeY;
		glfwGetWindowSize(window, &winSizeX, &winSizeY);
		ImVec2 windowPos((float)winPosX, (float)winPosY);
		ImVec2 windowSize((float)winSizeX, (float)winSizeY);
		// Set Mouse Position directly again from GLFW to ensure a valid mouse position, even, when the window is not active
		double mx, my;
		glfwGetCursorPos(window, &mx, &my);
		int wx, wy;
		glfwGetWindowPos(window, &wx, &wy);
		ImVec2 mouse = ImGui::GetIO().MousePos = ImVec2((float)mx + wx, (float)my + wy);

		// Set up title bar window
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(winPosX), static_cast<float>(winPosY)));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(winSizeX), titleBarHeight * dpiScale));
		ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
		ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background, since we use a global rounded background

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoDocking;

		ImGui::Begin("CustomTitleBar", nullptr, flags);

		// Draw Logo / Icon
		if (!logTextureID)
			logTextureID = LoadLogoTextureFromMemory(Icon32x32, Icon32x32_len);

		ImGui::SetCursorPos(ImVec2(padding / 2, (titleBarHeight * dpiScale - logoSize) * 0.5f + 2.0f));
		ImGui::Image(logTextureID, ImVec2(logoSize, logoSize));

		// Track if mouse is over any system button incl. menue buttons
		bool isOverSysButton = false;

		// Draw menu items
		float buttonY = (titleBarHeight * dpiScale - (ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y)) * 0.5f;
		ImGui::SetCursorPosY(buttonY);
		ImGui::SetCursorPosX(logoSize + padding);

		for (const auto& menu : menus)
		{
			DrawTopMenuItem(menu.name.c_str(), [&]()
				{
					for (const auto& item : menu.items)
					{
						if (item.isSeparator)
						{
							ImGui::Separator();
							continue;
						}
						const char* sc = item.shortcut ? item.shortcut->c_str() : nullptr;
						if (item.togglePtr)
							ImGui::MenuItem(item.label.c_str(), sc, item.togglePtr);
						else if (ImGui::MenuItem(item.label.c_str(), sc))
						{
							if (item.action != nullptr)
								item.action();
						}
					}
				}, isOverSysButton);
			ImGui::SetCursorPosY(buttonY); // Reset Y for next item — important due to SameLine()
		}

		// ==== Title bar buttons ====
		float buttonWidth = 45.0f * dpiScale; // DPI-aware sizes
		float buttonHeight = titleBarHeight * dpiScale;

		ImDrawList* draw = ImGui::GetWindowDrawList();

		// Loop over the 3 system buttons: [Minimize, Maximize/Restore, Close]
		for (int i = 0; i < 3; ++i)
		{
			float offsetX = windowSize.x - buttonWidth * (3 - i);
			ImVec2 buttonPos = windowPos + ImVec2(offsetX, 0.0f);
			ImVec2 buttonSize = ImVec2(buttonWidth, buttonHeight);

			// Adjust hitbox shrink depending on button index
			ImVec2 shrinkMin(0.0f, resizeBorder); // top shrink
			ImVec2 shrinkMax(0.0f, 0.0f);         // default no shrink right

			if (i == 2) // Close button -> shrink right edge
				shrinkMax.x = resizeBorder;

			ImRect hitbox(
				buttonPos + shrinkMin,
				buttonPos + buttonSize - shrinkMax
			);

			bool hovered = hitbox.Contains(mouse) && !isResizing;
			bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

			if (hovered)
				isOverSysButton = true;

			// Background
			ImU32 bgColor = IM_COL32(0, 0, 0, 0);
			if (hovered)
				bgColor = (i == 2) ? IM_COL32(232, 17, 35, 255) : IM_COL32(60, 60, 60, 255); // red for X, gray otherwise

			ImDrawFlags drawFlags = (i == 2 && !isMaximized) ? ImDrawFlags_RoundCornersTopRight : ImDrawFlags_None;
			float rounding = (i == 2 && !isMaximized) ? 10.0f * dpiScale : 0.0f;

			draw->AddRectFilled(buttonPos, buttonPos + buttonSize, bgColor, rounding, drawFlags);

			// Icon color
			ImU32 iconColor = hovered ? IM_COL32(255, 255, 255, 255) : IM_COL32(136, 136, 136, 255);

			// Icon center
			float cx = buttonPos.x + buttonSize.x * 0.5f;
			float cy = buttonPos.y + buttonSize.y * 0.5f;

			float iconSize = 5.5f * dpiScale;
			float lineThickness = 1.0f * dpiScale;

			// Draw icons
			switch (i)
			{
			case 0: // Minimize (_)
				draw->AddLine(
					ImVec2(cx - iconSize, cy),
					ImVec2(cx + iconSize, cy),
					iconColor, lineThickness);
				if (clicked)
					glfwIconifyWindow(window);
				break;

			case 1: // Maximize or Restore
				if (!isMaximized)
				{
					draw->AddRect(
						ImVec2(cx - iconSize, cy - iconSize),
						ImVec2(cx + iconSize, cy + iconSize),
						iconColor, 0.0f, 0, lineThickness);
				}
				else
				{
					float pad = 2.0f * dpiScale;

					// Back (top-right) partial box
					draw->AddLine(
						ImVec2(cx - iconSize + pad + lineThickness, cy - iconSize),
						ImVec2(cx + iconSize, cy - iconSize),
						iconColor, lineThickness);
					draw->AddLine(
						ImVec2(cx + iconSize, cy - iconSize),
						ImVec2(cx + iconSize, cy + iconSize - pad - lineThickness),
						iconColor, lineThickness);

					// Front (bottom-left) full box
					draw->AddRect(
						ImVec2(cx - iconSize, cy - iconSize + pad),
						ImVec2(cx + iconSize - pad, cy + iconSize),
						iconColor, 0.0f, 0, lineThickness);
				}

				if (clicked)
				{
					if (isMaximized)
						shouldRestoreWindow = true; // this should trigger restore to custom size
					else
						glfwMaximizeWindow(window);
				}
				break;

			case 2: // Close (X)
				draw->AddLine(ImVec2(cx - iconSize, cy - iconSize), ImVec2(cx + iconSize, cy + iconSize), iconColor, lineThickness);
				draw->AddLine(ImVec2(cx + iconSize, cy - iconSize), ImVec2(cx - iconSize, cy + iconSize), iconColor, lineThickness);
				if (clicked)
					glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			}

			if (hovered)
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		// ==== Resizing logic ====

		enum ResizeEdge {
			None,
			Left, Right, Top, Bottom,
			TopLeft, TopRight, BottomLeft, BottomRight
		};

		ResizeEdge hoveredEdge = ResizeEdge::None;

		// Detect edge or corner
		if (mouse.x >= winPosX && mouse.x <= winPosX + resizeBorder)
		{
			if (mouse.y >= winPosY && mouse.y <= winPosY + resizeBorder)
				hoveredEdge = TopLeft;
			else if (mouse.y >= winPosY + winSizeY - resizeBorder && mouse.y <= winPosY + winSizeY)
				hoveredEdge = BottomLeft;
			else
				hoveredEdge = Left;
		}
		else if (mouse.x >= winPosX + winSizeX - resizeBorder && mouse.x <= winPosX + winSizeX)
		{
			if (mouse.y >= winPosY && mouse.y <= winPosY + resizeBorder)
				hoveredEdge = TopRight;
			else if (mouse.y >= winPosY + winSizeY - resizeBorder && mouse.y <= winPosY + winSizeY)
				hoveredEdge = BottomRight;
			else
				hoveredEdge = Right;
		}
		else if (mouse.y >= winPosY && mouse.y <= winPosY + resizeBorder)
		{
			hoveredEdge = Top;
		}
		else if (mouse.y >= winPosY + winSizeY - resizeBorder && mouse.y <= winPosY + winSizeY)
		{
			hoveredEdge = Bottom;
		}

		// Set resize cursor
		if (!isOverSysButton)
		{
			switch (hoveredEdge)
			{
			case TopLeft:
			case BottomRight: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE); break;
			case TopRight:
			case BottomLeft:  ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW); break;
			case Left:
			case Right:       ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);   break;
			case Top:
			case Bottom:      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);   break;
			default: break;
			}
		}

		// Handle resizing
		static ResizeEdge activeEdge = None;
		static ImVec2 resizeStartMouse;
		static int resizeStartX = 0, resizeStartY = 0;
		static int resizeStartW = 0, resizeStartH = 0;

		if (hoveredEdge != None && !isOverSysButton && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			isResizing = true;
			activeEdge = hoveredEdge;
			resizeStartMouse = mouse;
			resizeStartX = winPosX;
			resizeStartY = winPosY;
			resizeStartW = winSizeX;
			resizeStartH = winSizeY;

			if (isMaximized)
			{
				// Make sure to restore maximized first
				shouldRestoreWindow = true;

				shouldMoveWindow = true;
				pendingMoveX = winPosX;
				pendingMoveY = winPosY;

				shouldResizeWindow = true;
				pendingResizeW = winSizeX;
				pendingResizeH = winSizeY;
			}
		}

		if (isResizing && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			ImVec2 delta = mouse - resizeStartMouse;
			int newX = resizeStartX;
			int newY = resizeStartY;
			int newW = resizeStartW;
			int newH = resizeStartH;
			const int minSize = 100;

			switch (activeEdge)
			{
			case Left:        newX += (int)delta.x; newW -= (int)delta.x; break;
			case Right:       newW += (int)delta.x; break;
			case Top:         newY += (int)delta.y; newH -= (int)delta.y; break;
			case Bottom:      newH += (int)delta.y; break;
			case TopLeft:     newX += (int)delta.x; newY += (int)delta.y; newW -= (int)delta.x; newH -= (int)delta.y; break;
			case TopRight:    newY += (int)delta.y; newW += (int)delta.x; newH -= (int)delta.y; break;
			case BottomLeft:  newX += (int)delta.x; newW -= (int)delta.x; newH += (int)delta.y; break;
			case BottomRight: newW += (int)delta.x; newH += (int)delta.y; break;
			default: break;
			}

			newW = std::max(newW, minSize);
			newH = std::max(newH, minSize);

			if (activeEdge == Left || activeEdge == Top || activeEdge == TopLeft || activeEdge == TopRight || activeEdge == BottomLeft)
			{
				shouldMoveWindow = true;
				pendingMoveX = newX;
				pendingMoveY = newY;
			}

			shouldResizeWindow = true;
			pendingResizeW = newW;
			pendingResizeH = newH;
		}

		if (isResizing && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			isResizing = false;
			activeEdge = None;
		}

		// Update flag for Resizing from Windows
		titleBarAllowDrag = (hoveredEdge == None) && !isOverSysButton;

		ImGui::End();

#elif defined(__APPLE__)
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		float titleBarHeight = 28.0f * dpiScale;
		float trafficLightsWidth = 80.0f * dpiScale; // reserve space for the mac traffic lights

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, titleBarHeight));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoDocking;

		ImGui::Begin("MacTitlebar", nullptr, flags);

		// --- DRAG ZONE (left-to-center), leaves space for traffic lights ---
		ImGui::SetCursorPos(ImVec2(trafficLightsWidth, 0.0f));
		ImGui::InvisibleButton("##MacDragZone", ImVec2(ImGui::GetContentRegionAvail().x - 200.0f * dpiScale, titleBarHeight));
		bool inDragZone = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

		// Double-click: maximize/zoom (or minimize if user pref says so)
		if (inDragZone && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			MacHandleTitlebarDoubleClick(window);
		}
		// Start drag on click (call once at press)
		else if (inDragZone && ImGui::IsItemActivated() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			MacBeginWindowDrag(window);
		}

		// --- Title centered ---
		const char* title = "Gear";
		ImVec2 textSize = ImGui::CalcTextSize(title);

		// center of whole window
		float centerX = (viewport->WorkSize.x - textSize.x) * 0.5f;

		// keep vertical centering
		float centerY = (titleBarHeight - textSize.y) * 0.5f;

		// apply position
		ImGui::SetCursorPos(ImVec2(centerX, centerY));
		ImGui::TextUnformatted(title);

		ImGui::End();
		ImGui::PopStyleVar(2);
#else
		// maybe Linux / stub
#endif
	}

	void GuiLayer::ShowMainWindow()
	{
		ImGui::Begin("Main");

		ImGui::SeparatorText("Font Tests");

		ImGui::PushFont(fontBold);
		ImGui::Text("Font bold example");
		ImGui::PopFont();

		ImGui::Text("%s Regular + Icon", ICON_FA_LIST_UL);

		ImGui::SeparatorText("Logging Tests");

		static const std::vector<std::function<void()>> logActions = {
			[] { LOG_INFO("Demo: User '{}' logged in.", "Alice"); },
			[] { LOG_DEBUG("Demo: Loading config file from '{}'.", "config/settings.json"); },
			[] { LOG_WARN("Demo: Low memory warning: {} MB remaining.", 128); },
			[] { LOG_ERROR("Demo: Failed to connect to server '{}:{}'", "api.example.com", 443); },
			[] { LOG_DEBUG("Demo: Initializing module: {}", "AudioEngine"); },
			[] { LOG_INFO("Demo: FPS counter initialized: {} FPS", 144); },
			[] { LOG_WARN("Demo: Frame took too long: {:.2f} ms", 45.3f); },
			[] { LOG_ERROR("Demo: Exception caught: {}", "std::runtime_error(\"Something broke\")"); },
		};

		static size_t index = 0;

		if (ImGui::Button("Generate Sample Log"))
		{
			logActions[index % logActions.size()]();
			++index;
		}

		if (ImGui::Button("Generate fix string logs"))
		{
			std::string fixStringLog = "Fix string log";
			LOG_DEBUG(fixStringLog);
			LOG_DEBUG("const char* log");
		}

		if (ImGui::Button("Generate 10k Logs"))
		{
			auto start = std::chrono::high_resolution_clock::now();

			for (int i = 0; i < 10000; ++i)
				LOG_DEBUG("Bulk log entry {}", i);

			auto end = std::chrono::high_resolution_clock::now();
			auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			LOG_INFO("Generated 10000 logs in {} ms", elapsedMs);
		}

		if (ImGui::Button("Benchmark (fmt-format, total 20k logs)"))
		{
			constexpr int totalLogs = 10000;

			auto logVariants = [](int i) {
				switch (i % 10)
				{
				case 0: LOG_INFO("Fixed string log"); break;
				case 1: LOG_WARN("Another fixed message"); break;
				case 2: LOG_DEBUG("Formatted int: {}, str: {}", 42, "debug msg"); break;
				case 3: LOG_ERROR("Float value: {:.3f}", 3.14159f); break;
				case 4: LOG1_INFO("Sensor", "Basic object log"); break;
				case 5: LOG1_WARN("Engine", "Speed dropped to {} RPM", 850); break;
				case 6: LOG2_ERROR("Motor", "Left", "Temp too high"); break;
				case 7: LOG2_DEBUG("Servo", "Right", "Reached position {}", 128); break;
				case 8: LOG3_INFO("Controller", "NodeX", "CamA", "Starting up"); break;
				case 9: LOG3_ERROR("MainLoop", "NodeY", "AxisZ", "Overload: {}A", 12.5f); break;
				}
				};

			// Single-threaded
			auto startSingle = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < totalLogs; ++i)
				logVariants(i);
			auto endSingle = std::chrono::high_resolution_clock::now();
			auto elapsedSingle = std::chrono::duration_cast<std::chrono::milliseconds>(endSingle - startSingle).count();

			// Multithreaded
			constexpr int numThreads = 4;
			int logsPerThread = totalLogs / numThreads;

			auto startMulti = std::chrono::high_resolution_clock::now();
			std::vector<std::thread> threads;
			for (int t = 0; t < numThreads; ++t)
			{
				threads.emplace_back([=]() {
					for (int i = 0; i < logsPerThread; ++i)
						logVariants(t * logsPerThread + i);
					});
			}
			for (auto& thread : threads) thread.join();
			auto endMulti = std::chrono::high_resolution_clock::now();
			auto elapsedMulti = std::chrono::duration_cast<std::chrono::milliseconds>(endMulti - startMulti).count();

			LOG_INFO("Benchmark fmt: Single-threaded: {} ms, Multi-threaded: {} ms", elapsedSingle, elapsedMulti);
		}

		if (ImGui::Button("Benchmark (manual concat, total 20k logs)"))
		{
			constexpr int totalLogs = 10000;

			auto logVariantsNoFmt = [](int i) {
				switch (i % 10)
				{
				case 0: LOG_INFO("Fixed string log"); break;
				case 1: LOG_WARN("Another fixed message"); break;
				case 2: LOG_DEBUG("Formatted: " + std::to_string(42) + " - debug"); break;
				case 3: LOG_ERROR("Float value: " + std::to_string(3.14159f)); break;
				case 4: LOG1_INFO("Sensor", "Manual object log"); break;
				case 5: LOG1_WARN("Engine", "Speed: " + std::to_string(850) + " RPM"); break;
				case 6: LOG2_ERROR("Motor", "Left", "Temp exceeded 85°C"); break;
				case 7: LOG2_DEBUG("Servo", "Right", "Reached: " + std::to_string(128)); break;
				case 8: LOG3_INFO("Main", "NodeX", "Cam", "Init module"); break;
				case 9: LOG3_ERROR("Loop", "AxisY", "MotorA", "Spike: " + std::to_string(12.5f) + "A"); break;
				}
				};

			// Single-threaded
			auto startSingle = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < totalLogs; ++i)
				logVariantsNoFmt(i);
			auto endSingle = std::chrono::high_resolution_clock::now();
			auto elapsedSingle = std::chrono::duration_cast<std::chrono::milliseconds>(endSingle - startSingle).count();

			// Multithreaded
			constexpr int numThreads = 4;
			int logsPerThread = totalLogs / numThreads;

			auto startMulti = std::chrono::high_resolution_clock::now();
			std::vector<std::thread> threads;
			for (int t = 0; t < numThreads; ++t)
			{
				threads.emplace_back([=]() {
					for (int i = 0; i < logsPerThread; ++i)
						logVariantsNoFmt(t * logsPerThread + i);
					});
			}
			for (auto& thread : threads) thread.join();
			auto endMulti = std::chrono::high_resolution_clock::now();
			auto elapsedMulti = std::chrono::duration_cast<std::chrono::milliseconds>(endMulti - startMulti).count();

			LOG_INFO("Benchmark no-fmt: Single-threaded: " + std::to_string(elapsedSingle) + " ms, Multi-threaded: " + std::to_string(elapsedMulti) + " ms");
		}

		ImGui::End();
	}

	void GuiLayer::ShowLoggerWindow()
	{
		ImGui::Begin("Logger");

		static bool autoScroll = true;
		ImGui::Checkbox("Auto Scroll", &autoScroll);

		static bool showFilter = false;
		static ImGuiTextFilter filter;
		ImGui::SameLine();
		ImGui::Checkbox("Enable Filter", &showFilter);

		const auto& buffer = Logger::GetBuffer();
		const size_t readIndex = Logger::GetReadIndex();
		const size_t logCount = Logger::GetSize();
		const size_t capacity = buffer.size();

		constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg;
		const float availHeight = ImGui::GetContentRegionAvail().y;
		static float levelWidth = ImGui::CalcTextSize("ERROR").x;
		static float timeWidth = ImGui::CalcTextSize("[2099:05:23 15:37:51.051]").x;
		float topHeight = showFilter ? (availHeight * 0.66f - ImGui::GetFrameHeightWithSpacing()) : availHeight;
		static size_t scrollToIndex = SIZE_MAX;

		auto toImVec4 = [](const LogMessageColor& c) -> ImVec4 {
			return ImVec4(c.r, c.g, c.b, c.a);
			};

		// Main log table (top)
		if (ImGui::BeginChild("##LogMain", ImVec2(0, topHeight), ImGuiChildFlags_Borders))
		{
			if (ImGui::BeginTable("LogTable", 3, tableFlags))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, levelWidth);
				ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, timeWidth);
				ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();

				if (scrollToIndex != SIZE_MAX)
				{
					int relativeRow = static_cast<int>((scrollToIndex + capacity - readIndex) % capacity);
					float rowHeight = ImGui::GetTextLineHeightWithSpacing();
					float targetY = relativeRow * rowHeight;
					float scrollY = std::max(0.0f, targetY - ImGui::GetWindowHeight() * 0.5f + rowHeight); // + rowHeight because of header!
					ImGui::SetScrollY(scrollY);
					scrollToIndex = SIZE_MAX;
				}

				ImGuiListClipper clipper;
				clipper.Begin(static_cast<int>(logCount));

				while (clipper.Step())
				{
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
					{
						// Calculate actual index in the ring buffer
						size_t bufferIndex = (readIndex + i) % capacity;
						const LogMessage& msg = buffer[bufferIndex];

						// Create time string
						char timeString[80];
						msg.FormatTimestamp(timeString, sizeof(timeString));

						ImGui::TableNextRow();

						ImGui::PushStyleColor(ImGuiCol_Text, toImVec4(msg.LevelColor()));

						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted(msg.FormatLevel());

						ImGui::TableSetColumnIndex(1);
						ImGui::TextUnformatted(timeString);

						ImGui::TableSetColumnIndex(2);
						ImGui::TextUnformatted(msg.message.c_str());

						ImGui::PopStyleColor();
					}
				}
				if (autoScroll && Logger::ShouldScrollToBottom())
					ImGui::SetScrollHereY(1.0f);
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();

		// Filtered log table (bottom)
		if (showFilter)
		{
			filter.Draw("Filter", 200.0f);

			static std::vector<size_t> filteredIndices;
			filteredIndices.clear();

			if (filter.IsActive())
			{
				for (size_t i = 0; i < logCount; ++i)
				{
					const size_t index = (readIndex + i) % capacity;
					const LogMessage& msg = buffer[index];
					if (filter.PassFilter(msg.message.c_str()))
						filteredIndices.push_back(index);
				}
			}

			if (ImGui::BeginChild("##Filtered", ImVec2(0, 0), ImGuiChildFlags_Borders))
			{
				if (ImGui::BeginTable("FilteredTable", 3, tableFlags))
				{
					ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, levelWidth);
					ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, timeWidth);
					ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);

					ImGuiListClipper clipper;
					clipper.Begin(static_cast<int>(filteredIndices.size()));

					while (clipper.Step())
					{
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
						{
							// Get the filtered entry
							const size_t index = filteredIndices[i];
							const LogMessage& msg = buffer[index];

							// Create time string
							char timeString[80];
							msg.FormatTimestamp(timeString, sizeof(timeString));

							ImGui::TableNextRow();

							ImGui::PushStyleColor(ImGuiCol_Text, toImVec4(msg.LevelColor()));

							ImGui::TableSetColumnIndex(0);
							ImGui::TextUnformatted(msg.FormatLevel());

							ImGui::TableSetColumnIndex(1);
							ImGui::TextUnformatted(timeString);

							ImGui::TableSetColumnIndex(2);
							ImGui::TextUnformatted(msg.message.c_str());
							if (ImGui::IsItemClicked())
								scrollToIndex = index;

							ImGui::PopStyleColor();
						}
					}
					ImGui::EndTable();
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}
}
