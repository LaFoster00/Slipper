#include "Input.h"

#include "AppEvents.h"
#include "Application.h"
#include "InputEvent.h"
#include "Window/Window.h"

namespace Slipper
{
void InputManager::RegisterInputCallbacks(const Window &Window)
{
    glfwSetKeyCallback(Window, Key_Callback);

    glfwSetCursorEnterCallback(Window, CursorEnter_Callback);
    glfwSetCursorPosCallback(Window, CursorPos_Callback);
    glfwSetMouseButtonCallback(Window, MouseButton_Callback);
    glfwSetScrollCallback(Window, Scroll_Callback);

    glfwSetWindowCloseCallback(Window, WindowClose_Callback);
    glfwSetWindowMaximizeCallback(Window, WindowMaximize_Callback);
    glfwSetWindowFocusCallback(Window, WindowFocus_Callback);

    glfwSetFramebufferSizeCallback(Window, FramebufferResize_Callback);
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
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));

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
}

IMPLEMENT_GLFW_CALLBACK(Scroll, double XOffset, double YOffset)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
        MouseScrolledEvent event(XOffset, YOffset);
        window.m_eventCallback(event);
    }
}

IMPLEMENT_GLFW_CALLBACK(CursorEnter, int Entered)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
        CursorEnterEvent event(window, Entered);
        window.m_eventCallback(event);
    }
}

IMPLEMENT_GLFW_CALLBACK(CursorPos, double XPos, double YPos)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
        MouseMovedEvent event(XPos, YPos);
        window.m_eventCallback(event);
    }
}

IMPLEMENT_GLFW_CALLBACK(MouseButton, int Button, int Action, int Mods)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
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