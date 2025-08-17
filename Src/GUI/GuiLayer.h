#pragma once

#include <functional>

struct GLFWwindow;

namespace gear
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

		float GetDpiScale() { return dpiScale; }
		bool GetIsMaximized() { return isMaximized; }
		bool GetTitleBarAllowDrag() { return titleBarAllowDrag; }

	private:
		void ApplyCustomDarkTheme();
		void DrawTopMenuItem(const char* label, const std::function<void()>& contentFn, bool& isOverSysButton);
		void RenderCustomTitleBar(GLFWwindow* window);

		void ShowMainWindow();
		void ShowLoggerWindow();

		float dpiScale = 1.0f;
		bool isMaximized = false;
		bool titleBarAllowDrag = false;

		bool showImGuiDemoWindow = false;

		// Helper to move / resize window
		bool shouldRestoreWindow = false;
		bool shouldMoveWindow = false;
		int pendingMoveX = 0, pendingMoveY = 0;
		bool shouldResizeWindow = false;
		int pendingResizeW = 0, pendingResizeH = 0;
	};
}
