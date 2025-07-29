#pragma once

struct GLFWwindow;

namespace cppsandbox
{
    class GuiLayer
    {
    public:
        void Init(GLFWwindow* window);
        void Shutdown();

        void BeginFrame();
        void EndFrame(GLFWwindow* window);

    private:
        void ApplyCustomDarkTheme();
    };
}
