#pragma once

#include "GUI/GuiLayer.h"

namespace gear
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();

	private:
		void Init();
		void Shutdown();

		GLFWwindow* window = nullptr;
		GuiLayer guiLayer;
	};
}
