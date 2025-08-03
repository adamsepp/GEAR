#include <GLFW/glfw3.h>
#include <functional>
#include <chrono>

#include "GuiLayer.h"
#include "Logger/Logger.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace cppsandbox
{
	void GuiLayer::Init(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ApplyCustomDarkTheme();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void GuiLayer::Shutdown()
	{
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
		// Begin a new ImGui frame
		// ------------------------------------------------------------
		ImGui_ImplOpenGL3_NewFrame();   // Prepare OpenGL3 bindings
		ImGui_ImplGlfw_NewFrame();      // Prepare GLFW input bindings
		ImGui::NewFrame();              // Begin ImGui frame logic

		// ------------------------------------------------------------
		// Draw a rounded window background behind all ImGui windows,
		// unless the GLFW window is maximized, or the platform doesn't support it (e.g. Linux without compositor).
		// ------------------------------------------------------------
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImDrawList* drawList = ImGui::GetBackgroundDrawList(const_cast<ImGuiViewport*>(viewport));

		ImVec2 pos = viewport->Pos;
		ImVec2 size = viewport->Size;

		bool isMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;

		// ---------------- Platform-dependent Rounding ----------------
#if defined(__linux__)
		// Rounded corners are disabled on Linux for now, because transparency support
		// (e.g. via compositing or 32-bit visual) is not guaranteed and often fails
		// TODO: Detect compositor or enable rounding if supported (e.g. with Wayland or picom) - currently disabled in CMakeLists
		float rounding = 0.0f;
#else
		float rounding = isMaximized ? 0.0f : 8.0f;
#endif
		ImDrawFlags drawFlags = isMaximized ? ImDrawFlags_None : ImDrawFlags_RoundCornersAll;
		ImU32 bgColor = ImGui::GetColorU32(ImGuiCol_WindowBg);

		// Draw filled background
		drawList->AddRectFilled(
			pos,
			ImVec2(pos.x + size.x, pos.y + size.y),
			bgColor,
			rounding,
			drawFlags
		);

		// Optional border for non-maximized windows
		if (!isMaximized && drawFlags != ImDrawFlags_None)
		{
			drawList->AddRect(
				pos,
				ImVec2(pos.x + size.x, pos.y + size.y),
				ImGui::GetColorU32(ImGuiCol_Border),
				rounding,
				drawFlags,
				1.0f // border thickness
			);
		}
	}

	void GuiLayer::Render(GLFWwindow* window)
	{
		// Draw cutom title bar
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
			ImGui::DockBuilderDockWindow("CppSandbox", topDockID);
			ImGui::DockBuilderDockWindow("Logger", bottomDockID);
			ImGui::DockBuilderDockWindow("Dear ImGui Demo", topDockID);

			ImGui::DockBuilderFinish(dockspaceID);
		}

		// Render docked windows
		ShowSandboxWindow();
		ShowLoggerWindow();

		if (showImGuiDemoWindow)
			ImGui::ShowDemoWindow(&showImGuiDemoWindow);
	}

	void GuiLayer::EndFrame(GLFWwindow* window)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
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

	void GuiLayer::DrawTopMenuItem(const char* label, const std::function<void()>& contentFn)
	{
		static const char* activeTopMenuName = nullptr;

		float paddingX = ImGui::GetStyle().FramePadding.x * 2;
		ImVec2 labelSize = ImGui::CalcTextSize(label);
		float buttonHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		float buttonWidth = labelSize.x + paddingX * 2.0f;

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 size = ImVec2(buttonWidth, buttonHeight);

		bool hovered = ImGui::IsMouseHoveringRect(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		bool isActive = (activeTopMenuName != nullptr && strcmp(activeTopMenuName, label) == 0);

		// Highlight background if the item is hovered or active
		if (hovered || isActive)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(
				pos,
				ImVec2(pos.x + size.x, pos.y + size.y),
				IM_COL32(60, 60, 60, 255)
			);
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
		}

		// Hover behavior: switch active menu if another is already open
		if (hovered && activeTopMenuName != nullptr && !isActive)
		{
			activeTopMenuName = label;
			ImGui::OpenPopup(label);
		}

		// Position the popup directly below the menu item
		ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + size.y - 1.0f));

		// Render the popup if it's open
		if (ImGui::BeginPopup(label))
		{
			contentFn();
			ImGui::EndPopup();
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
		// DPI-aware values
		float dpiScale = ImGui::GetIO().DisplayFramebufferScale.y;
		float padding = 8.0f * dpiScale;
		float logoSize = (titleBarHeight - 10.0f) * dpiScale; // Dynamically scale logo with title bar height

		int winX, winY;
		glfwGetWindowPos(window, &winX, &winY);
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		// Set up title bar window
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(winX), static_cast<float>(winY)));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(w), titleBarHeight * dpiScale));
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

		// Logo placeholder
		ImGui::SetCursorPos(ImVec2(padding, (titleBarHeight * dpiScale - logoSize) * 0.5f));
		{
			ImVec2 logoPos = ImGui::GetCursorScreenPos();
			ImVec2 logoSizeVec = ImVec2(logoSize, logoSize);
			ImGui::Dummy(logoSizeVec);
			ImGui::GetWindowDrawList()->AddRectFilled(
				logoPos,
				ImVec2(logoPos.x + logoSizeVec.x, logoPos.y + logoSizeVec.y),
				IM_COL32(100, 100, 100, 255),
				4.0f);
			ImGui::GetWindowDrawList()->AddRect(
				logoPos,
				ImVec2(logoPos.x + logoSizeVec.x, logoPos.y + logoSizeVec.y),
				IM_COL32(50, 50, 50, 255),
				4.0f);
			ImVec2 textSize = ImGui::CalcTextSize("T");
			ImVec2 textPos = ImVec2(
				logoPos.x + (logoSize - textSize.x) * 0.5f,
				logoPos.y + (logoSize - textSize.y) * 0.5f);
			ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255), "T");
		}

		// Draw menu items
		float buttonY = (titleBarHeight * dpiScale - (ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f)) * 0.5f;
		ImGui::SetCursorPosY(buttonY);
		ImGui::SetCursorPosX(logoSize + 2 * padding);

		// Draw Menues
		DrawTopMenuItem("File", [&]()
			{
				if (ImGui::MenuItem("Exit"))
					glfwSetWindowShouldClose(window, GLFW_TRUE);
			});

		// Reset Y for next item — important due to SameLine()
		ImGui::SetCursorPosY(buttonY);
		DrawTopMenuItem("View", [&]()
			{
				ImGui::MenuItem("Show ImGui Demo", nullptr, &showImGuiDemoWindow);
			});

		// TODO: Minimize button
		// TODO: maximize/restore button

		// Close button (right-aligned and vertically centered)
		float closeButtonWidth = ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float closeButtonHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

		ImGui::SetCursorPosY((titleBarHeight * dpiScale - closeButtonHeight) * 0.5f);
		ImGui::SetCursorPosX(static_cast<float>(w) - closeButtonWidth - padding);

		if (ImGui::Button("X"))
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		// ==== Resizing logic ====

		const float resizeBorder = 5.0f;
		ImVec2 mouse = ImGui::GetMousePos();

		enum ResizeEdge {
			None,
			Left, Right, Top, Bottom,
			TopLeft, TopRight, BottomLeft, BottomRight
		};

		ResizeEdge hoveredEdge = ResizeEdge::None;

		// Detect edge or corner
		if (mouse.x >= winX && mouse.x <= winX + resizeBorder)
		{
			if (mouse.y >= winY && mouse.y <= winY + resizeBorder)
				hoveredEdge = TopLeft;
			else if (mouse.y >= winY + h - resizeBorder && mouse.y <= winY + h)
				hoveredEdge = BottomLeft;
			else
				hoveredEdge = Left;
		}
		else if (mouse.x >= winX + w - resizeBorder && mouse.x <= winX + w)
		{
			if (mouse.y >= winY && mouse.y <= winY + resizeBorder)
				hoveredEdge = TopRight;
			else if (mouse.y >= winY + h - resizeBorder && mouse.y <= winY + h)
				hoveredEdge = BottomRight;
			else
				hoveredEdge = Right;
		}
		else if (mouse.y >= winY && mouse.y <= winY + resizeBorder)
		{
			hoveredEdge = Top;
		}
		else if (mouse.y >= winY + h - resizeBorder && mouse.y <= winY + h)
		{
			hoveredEdge = Bottom;
		}

		// Set resize cursor
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

		// Handle resizing
		static bool isResizing = false;
		static ResizeEdge activeEdge = None;
		static ImVec2 resizeStartMouse;
		static int resizeStartX = 0, resizeStartY = 0;
		static int resizeStartW = 0, resizeStartH = 0;

		if (hoveredEdge != None && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			isResizing = true;
			activeEdge = hoveredEdge;
			resizeStartMouse = mouse;
			resizeStartX = winX;
			resizeStartY = winY;
			resizeStartW = w;
			resizeStartH = h;
		}

		if (isResizing && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			ImVec2 delta = ImVec2(mouse.x - resizeStartMouse.x, mouse.y - resizeStartMouse.y);
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
				glfwSetWindowPos(window, newX, newY);

			glfwSetWindowSize(window, newW, newH);
		}

		if (isResizing && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			isResizing = false;
			activeEdge = None;
		}

		// ==== Dragging (only when not resizing) ====

		static bool dragging = false;
		static ImVec2 dragStartMousePos;
		static int dragStartWinX, dragStartWinY;
		bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
		bool overItem = ImGui::IsAnyItemHovered();
		bool inResizeZone = hoveredEdge != None;

		if (!isResizing && !inResizeZone && hovered && !overItem && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			dragging = true;
			dragStartMousePos = ImGui::GetMousePos();
			dragStartWinX = winX;
			dragStartWinY = winY;
		}

		if (dragging && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
			dragging = false;

		if (dragging)
		{
			ImVec2 delta = ImVec2(mouse.x - dragStartMousePos.x, mouse.y - dragStartMousePos.y);
			glfwSetWindowPos(window,
				dragStartWinX + static_cast<int>(delta.x),
				dragStartWinY + static_cast<int>(delta.y));
		}

		// Optional: maximize/restore on double-click
		if (!isResizing && !inResizeZone && hovered && !overItem && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			// TODO: Toggle window maximization state here
		}

		ImGui::End();
	}

	void GuiLayer::ShowSandboxWindow()
	{
		ImGui::Begin("CppSandbox");

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
