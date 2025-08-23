#pragma once
#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C" {
#endif

	void MacSetupMenuAndTitlebar(GLFWwindow* win);
	void MacSyncMenusFromGuiLayer(gear::GuiLayer* layer);

#ifdef __cplusplus
}
#endif
