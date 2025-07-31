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

	void GuiLayer::BeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GuiLayer::Render()
	{
		static bool showImGuiDemoWindow = false;

		// Main DockSpace + Menu Bar
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("MainDockSpace", nullptr, windowFlags);
		ImGui::PopStyleVar(2);

		ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Show ImGui Demo", nullptr, &showImGuiDemoWindow);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();

		// One-time setup of dock layout
		static bool dockInitialized = false;
		if (!dockInitialized)
		{
			dockInitialized = true;

			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->WorkSize);

			ImGuiID dockMainID = dockspaceID;
			ImGuiID bottomDockID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.3f, nullptr, &dockMainID);
			ImGuiID topDockID = dockMainID;

			ImGui::DockBuilderDockWindow("CppSandbox", topDockID);
			ImGui::DockBuilderDockWindow("Logger", bottomDockID);
			ImGui::DockBuilderDockWindow("Dear ImGui Demo", topDockID);

			ImGui::DockBuilderFinish(dockspaceID);
		}

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

		const auto& buffer = Logger::GetBuffer();
		const size_t readIndex = Logger::GetReadIndex();
		const size_t logCount = Logger::GetSize();
		const size_t capacity = buffer.size();

		constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg;
		const float availHeight = ImGui::GetContentRegionAvail().y;
		static float levelWidth = ImGui::CalcTextSize("ERROR").x;
		static float timeWidth = ImGui::CalcTextSize("[2099:05:23 15:37:51.051]").x;

		if (ImGui::BeginTable("LogTable", 3, tableFlags, ImVec2(0, availHeight)))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, levelWidth);
			ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, timeWidth);
			ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			ImGuiListClipper clipper;
			clipper.Begin(static_cast<int>(logCount));

			auto toImVec4 = [](const LogMessageColor& c) -> ImVec4 {
				return ImVec4(c.r, c.g, c.b, c.a);
				};

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

					ImGui::TableSetColumnIndex(0);
					ImGui::PushStyleColor(ImGuiCol_Text, toImVec4(msg.LevelColor()));
					ImGui::TextUnformatted(msg.FormatLevel());
					ImGui::PopStyleColor();

					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(timeString);

					ImGui::TableSetColumnIndex(2);
					ImGui::TextUnformatted(msg.message.c_str());
				}
			}

			if (autoScroll && Logger::ShouldScrollToBottom())
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndTable();
		}

		ImGui::End();
	}
}
