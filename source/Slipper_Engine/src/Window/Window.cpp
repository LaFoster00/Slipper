#include "Window.h"

#include "GraphicsEngine.h"

Window::Window(WindowInfo CreateInfo)
{
    m_info = CreateInfo;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, CreateInfo.resizable);

    glfwWindow = glfwCreateWindow(m_info.width, m_info.height, m_info.name, nullptr, nullptr);

    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, FramebufferResizeCallback);
}

void Window::FramebufferResizeCallback(GLFWwindow *Window, int Width, int Height)
{
    GraphicsEngine::Get().OnWindowResized();
}