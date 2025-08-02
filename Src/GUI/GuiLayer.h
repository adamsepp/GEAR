#pragma once

#include <functional>

struct GLFWwindow;

namespace cppsandbox
{
	constexpr float titleBarHeight = 40.0f;

	class GuiLayer
	{
	public:
		void Init(GLFWwindow* window);
		void Shutdown();

		void BeginFrame();
		void Render(GLFWwindow* window);
		void EndFrame(GLFWwindow* window);

	private:
		void ApplyCustomDarkTheme();
		void DrawTopMenuItem(const char* label, const std::function<void()>& contentFn);
		void RenderCustomTitleBar(GLFWwindow* window);

		void ShowSandboxWindow();
		void ShowLoggerWindow();

		bool showImGuiDemoWindow = false;
	};
}
