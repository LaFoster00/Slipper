#include "Window.h"

#include <ranges>

#include "Core/Application.h"
#include "GraphicsEngine.h"
#include "Presentation/Surface.h"

namespace Slipper
{
std::unordered_map<GLFWwindow *, Window *> Window::m_windows{};

Window::Window(WindowInfo CreateInfo)
{
    m_info = CreateInfo;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, CreateInfo.resizable);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    glfwWindow = glfwCreateWindow(m_info.width, m_info.height, m_info.name, nullptr, nullptr);

    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, FramebufferResizeCallback);

    m_windows[glfwWindow] = this;

    m_surface = std::make_unique<Surface>(*this);
}

VkExtent2D Window::GetSize() const
{
    auto window = m_windows.at(glfwWindow);
    return {window->m_info.width, window->m_info.height};
}

void Window::OnUpdate()
{
    if (glfwWindowShouldClose(glfwWindow)) {
        Slipper::Application::Get().Close();
    }

    glfwPollEvents();
}

void Window::FramebufferResizeCallback(GLFWwindow *Window, int Width, int Height)
{
    Slipper::Window *abstract_window = m_windows.at(Window);

    Application::Get()
        .OnWindowResize(abstract_window, Width, Height);

    abstract_window->m_info.width = Width;
    abstract_window->m_info.height = Height;
}
}