#include <GLFW/glfw3.h>

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

        // Main window content
        ImGui::Begin("CppSandbox");
        ImGui::Text("Hello from the CppSandbox application!");
        ImGui::End();

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

    void GuiLayer::ShowLoggerWindow()
    {
        ImGui::Begin("Logger");

        std::vector<LogMessage> messages = Logger::GetSnapshot();

        ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0));

        for (const LogMessage& msg : messages)
        {
            ImVec4 color;

            switch (msg.level)
            {
            case LogLevel::Info:    color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break;
            case LogLevel::Warning: color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); break;
            case LogLevel::Error:   color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); break;
            case LogLevel::Debug:   color = ImVec4(0.5f, 0.5f, 1.0f, 1.0f); break;
            }

            std::time_t time = std::chrono::system_clock::to_time_t(msg.timestamp);
            std::tm tm = *std::localtime(&time);
            char timeBuffer[9];
            std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &tm);

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("[%s] %s", timeBuffer, msg.message.c_str());
            ImGui::PopStyleColor();
        }

        ImGui::EndChild();
        ImGui::End();
    }
}
