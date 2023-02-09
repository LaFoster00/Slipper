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

glm::vec2 Window::GetSize() const
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

void Window::AddResizeCallback(void *Context, std::function<void(Window &, int, int)> Callback)
{
    m_resizeCallbacks[Context] = Callback;
}

void Window::RemoveResizeCallback(void *Context, std::function<void(Window &, int, int)> Callback)
{
    if (!m_resizeCallbacks.contains(Context))
        return;

    m_resizeCallbacks.erase(Context);
}

void Window::FramebufferResizeCallback(GLFWwindow *Window, int Width, int Height)
{
    GraphicsEngine::Get().OnWindowResized();

    Slipper::Window *abstract_window = m_windows.at(Window);
    abstract_window->m_info.width = Width;
    abstract_window->m_info.height = Height;
    for (auto &callback : abstract_window->m_resizeCallbacks | std::views::values) {
        callback(*abstract_window, Width, Height);
    }
}
}