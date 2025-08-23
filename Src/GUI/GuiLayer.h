#pragma once
#include <functional>
#include <string>
#include <vector>
#include <optional>

struct GLFWwindow;

namespace gear
{
	struct MenuItem
	{
		std::string label;
		std::optional<std::string> shortcut;
		std::function<void()> action;
		bool isSeparator = false;
		bool* togglePtr = nullptr;

		static MenuItem Separator() { return MenuItem{ "", std::nullopt, {}, true }; }
	};

	struct MenuDef
	{
		std::string name;
		std::vector<MenuItem> items;
	};

	class GuiLayer
	{
	public:
		void Init(GLFWwindow* window);
		void Shutdown();

		void BeginFrame(GLFWwindow* window);
		void Render(GLFWwindow* window);
		void EndFrame(GLFWwindow* window);

		// Registry-API:
		void AddMenu(MenuDef menu);
		void RegisterDefaultMenus(GLFWwindow* window);
		const std::vector<MenuDef>& Menus() const { return menus; }

		float GetTitleBarHeight() const { return titleBarHeight; }
		float GetDpiScale() const { return dpiScale; }
		bool GetIsMaximized() const { return isMaximized; }
		bool GetTitleBarAllowDrag() const { return titleBarAllowDrag; }

	private:
		// Drawing
		void ApplyCustomDarkTheme();
		void DrawTopMenuItem(const char* label, const std::function<void()>& contentFn, bool& isOverSysButton);
		void RenderCustomTitleBar(GLFWwindow* window);

		void ShowMainWindow();
		void ShowLoggerWindow();

		// State
		float titleBarHeight = 32.0f; // Windows Standard
		float dpiScale = 1.0f;
		bool isMaximized = false;
		bool titleBarAllowDrag = false;

		bool showImGuiDemoWindow = false;

		// Menu-Registry
		std::vector<MenuDef> menus;

		// Helper to move / resize window
		bool shouldRestoreWindow = false;
		bool shouldMoveWindow = false;
		int pendingMoveX = 0, pendingMoveY = 0;
		bool shouldResizeWindow = false;
		int pendingResizeW = 0, pendingResizeH = 0;
	};
}
