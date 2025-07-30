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
        void Render();
        void EndFrame(GLFWwindow* window);

    private:
        void ApplyCustomDarkTheme();

        void ShowSandboxWindow();
        void ShowLoggerWindow();
    };
}
