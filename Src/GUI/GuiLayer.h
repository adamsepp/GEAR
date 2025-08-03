#pragma once

#include <functional>

struct GLFWwindow;

namespace cppsandbox
{
	constexpr float titleBarHeight = 32.0f; // Windows Standard

	class GuiLayer
	{
	public:
		void Init(GLFWwindow* window);
		void Shutdown();

		void BeginFrame(GLFWwindow* window);
		void Render(GLFWwindow* window);
		void EndFrame(GLFWwindow* window);

	private:
		void ApplyCustomDarkTheme();
		void DrawTopMenuItem(const char* label, const std::function<void()>& contentFn);
		void RenderCustomTitleBar(GLFWwindow* window);

		void ShowSandboxWindow();
		void ShowLoggerWindow();

		bool showImGuiDemoWindow = false;

		// Helper to move / resize window
		bool shouldMoveWindow = false;
		int pendingMoveX = 0, pendingMoveY = 0;
		bool shouldResizeWindow = false;
		int pendingResizeW = 0, pendingResizeH = 0;
	};
}
