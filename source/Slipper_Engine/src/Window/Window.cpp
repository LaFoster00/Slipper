#include "Window.h"

#include <ranges>

#include "Core/AppEvents.h"
#include "Core/Application.h"
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

    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetKeyCallback(glfwWindow, Key_Callback);

    glfwSetCursorEnterCallback(glfwWindow, CursorEnter_Callback);
    glfwSetCursorPosCallback(glfwWindow, CursorPos_Callback);
    glfwSetMouseButtonCallback(glfwWindow, MouseButton_Callback);
    glfwSetScrollCallback(glfwWindow, Scroll_Callback);

    glfwSetWindowCloseCallback(glfwWindow, WindowClose_Callback);
    glfwSetWindowMaximizeCallback(glfwWindow, WindowMaximize_Callback);
    glfwSetWindowFocusCallback(glfwWindow, WindowFocus_Callback);

    glfwSetFramebufferSizeCallback(glfwWindow, FramebufferResize_Callback);

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

IMPLEMENT_GLFW_CALLBACK(FramebufferResize, int Width, int Height)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));

    Application::Get().OnWindowResize(&window, Width, Height);

    window.m_info.width = Width;
    window.m_info.height = Height;
}

IMPLEMENT_GLFW_CALLBACK(Key, int Key, int Scancode, int Action, int Mods)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));

    switch (Action) {
        case GLFW_PRESS: {
            KeyPressedEvent event(Key, false);
            window.m_eventCallback(event);
            break;
        }
        case GLFW_RELEASE: {
            KeyReleasedEvent event(Key);
            window.m_eventCallback(event);
            break;
        }
        case GLFW_REPEAT: {
            KeyPressedEvent event(Key, true);
            window.m_eventCallback(event);
            break;
        }
        default:;
    }
}

IMPLEMENT_GLFW_CALLBACK(Scroll, double XOffset, double YOffset)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    MouseScrolledEvent event(XOffset, YOffset);
    window.m_eventCallback(event);
}

IMPLEMENT_GLFW_CALLBACK(CursorEnter, int Entered)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    CursorEnterEvent event(window, Entered);
    window.m_eventCallback(event);
}

IMPLEMENT_GLFW_CALLBACK(CursorPos, double XPos, double YPos)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    MouseMovedEvent event(XPos, YPos);
    window.m_eventCallback(event);
}

IMPLEMENT_GLFW_CALLBACK(MouseButton, int Button, int Action, int Mods)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    switch (Action) {
        case GLFW_PRESS: {
            MouseButtonPressedEvent event(Button);
            window.m_eventCallback(event);
            break;
        }
        case GLFW_RELEASE: {
            MouseButtonReleasedEvent event(Button);
            window.m_eventCallback(event);
            break;
        }
        default:;
    }
}

IMPLEMENT_GLFW_CALLBACK(WindowMaximize, int Maximized)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    WindowMaximizeEvent event(window, Maximized);
    window.m_eventCallback(event);
}

IMPLEMENT_GLFW_CALLBACK(WindowFocus, int Focused)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    WindowFocusEvent event(window, Focused);
    window.m_eventCallback(event);
}

IMPLEMENT_GLFW_DIRECT_CALLBACK(WindowClose)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    WindowCloseEvent event(window);
    window.m_eventCallback(event);
}
}  // namespace Slipper