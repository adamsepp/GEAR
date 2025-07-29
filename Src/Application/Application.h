#pragma once

#include <GLFW/glfw3.h>

namespace cppsandbox {

class Application {
public:
    Application();
    ~Application();

    void Run();

private:
    void Init();
    void Shutdown();

    struct GLFWwindow* m_Window = nullptr;
};

} // namespace cppsandbox
