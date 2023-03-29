#include "Window.h"

#include "Core/InputEvent.h"
#include "Presentation/Surface.h"

namespace Slipper
{
Window::Window(WindowInfo CreateInfo)
{
    m_info = CreateInfo;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, CreateInfo.resizable);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    glfwWindow = glfwCreateWindow(
        m_info.width, m_info.height, m_info.name.c_str(), nullptr, nullptr);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetWindowUserPointer(glfwWindow, this);

    InputManager::RegisterInputCallbacks(*this);

    m_surface = std::make_unique<Surface>(*this);
}

Window::~Window()
{
    vkDeviceWaitIdle(Device::Get());
    m_surface.reset();
    glfwDestroyWindow(glfwWindow);
}

void Window::SetTitle(std::string_view Name)
{
    m_info.name = Name;
    glfwSetWindowTitle(glfwWindow, m_info.name.c_str());
}

void Window::OnUpdate()
{
    glfwPollEvents();
}
}  // namespace Slipper