#include "Window.h"

#include "Context.h"
#include "InputEvent.h"

namespace Slipper
{
    Window::Window(WindowInfo CreateInfo)
    {
        m_info = CreateInfo;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, CreateInfo.resizable);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

        glfwWindow = glfwCreateWindow(m_info.width, m_info.height, m_info.name.c_str(), nullptr, nullptr);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        glfwSetWindowUserPointer(glfwWindow, this);

        InputManager::RegisterGlfwInputCallbacks(*this);

        m_context = std::unique_ptr<GPU::Context>(GPU::Context::CreateContext(*this));
    }

    Window::~Window()
    {
        m_context->WaitIdle();

        m_context.reset();
        glfwDestroyWindow(glfwWindow);
    }

    void Window::SetTitle(std::string_view Name)
    {
        m_info.name = Name;
        glfwSetWindowTitle(glfwWindow, m_info.name.c_str());
    }

    glm::int2 Window::GetResolution() const
    {
        return m_context->GetResolution();
    }

    void Window::OnUpdate()
    {
        glfwPollEvents();
    }
}  // namespace Slipper
