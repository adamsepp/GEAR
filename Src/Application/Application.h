#pragma once

#include "GUI/GuiLayer.h"

namespace cppsandbox
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

    private:
        GLFWwindow* window = nullptr;
        GuiLayer guiLayer;
    };

} // namespace cppsandbox
