#include "Input.h"

#include <imgui_internal.h>

#include "AppEvents.h"
#include "Application.h"
#include "InputEvent.h"
#include "Window/Window.h"

namespace Slipper
{
bool Input::GetMouseButtonDown(MouseCode Button)
{
    return mouseInput.buttons[static_cast<size_t>(Button)].down;
}

bool Input::GetMouseButtonReleased(MouseCode Button)
{
    return !mouseInput.buttons[static_cast<size_t>(Button)].down &&
           mouseInput.buttons[static_cast<size_t>(Button)].changed;
}

bool Input::GetMouseButtonPressed(MouseCode Button)
{
    return mouseInput.buttons[static_cast<size_t>(Button)].down &&
           mouseInput.buttons[static_cast<size_t>(Button)].changed;
}

void Input::CaptureMouse(bool Capture)
{
    if (Capture == captureMouseCursor)
        return;

    glfwSetInputMode(Application::Get().window->glfwWindow,
                     GLFW_CURSOR,
                     Capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    if (Capture) {
        mouseInput.preCapturePosition = mouseInput.position;
    }
    else {
        mouseInput.position = mouseInput.preCapturePosition;
        mouseInput.movement = {};
    }

    captureMouseCursor = Capture;
}

glm::vec2 Input::GetMouseMovement()
{
    // LOG_FORMAT("Entered window : {}", enteredWindow)
    if (!captureMouseCursor && (!insideWindow || enteredWindow)) {
        return {};
    }
    else {
        return mouseInput.movement;
    }
}

bool Input::GetKeyDown(KeyCode Key)
{
    return keyInputs[static_cast<size_t>(Key)].down;
}

bool Input::GetKeyReleased(KeyCode Key)
{
    return !keyInputs[static_cast<size_t>(Key)].down &&
           keyInputs[static_cast<size_t>(Key)].changed;
}

bool Input::GetKeyPressed(KeyCode Key)
{
    return keyInputs[static_cast<size_t>(Key)].down && keyInputs[static_cast<size_t>(Key)].changed;
}

void Input::UpdateInputs()
{
    for (auto &data : mouseInput.buttons) {
        data.changed = false;
    }
    mouseInput.movement = {};

    for (auto &data : keyInputs) {
        data.changed = false;
    }
}

void InputManager::SetImGuiInputContext(NonOwningPtr<ImGuiContext> Context)
{
    context = Context;
}

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

void InputManager::SetInputOffset(glm::vec2 Offset)
{
    InputManager::Offset = Offset;
}

IMPLEMENT_GLFW_CALLBACK(FramebufferResize, int Width, int Height)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));

    Application::Get().OnWindowResize(&window, Width, Height);

    window.m_info.width = Width;
    window.m_info.height = Height;
    auto io = ImGui::GetIO();
}

IMPLEMENT_GLFW_CALLBACK(Key, int Key, int Scancode, int Action, int Mods)
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse || Input::captureMouseCursor) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));

        switch (Action) {
            case GLFW_PRESS: {
                Input::keyInputs[Key].down = true;
                Input::keyInputs[Key].changed = true;
                KeyPressedEvent event(static_cast<KeyCode>(Key), false);
                window.m_eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                Input::keyInputs[Key].down = false;
                Input::keyInputs[Key].changed = true;
                KeyReleasedEvent event(static_cast<KeyCode>(Key));
                window.m_eventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(static_cast<KeyCode>(Key), true);
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
    if (!io.WantCaptureMouse || Input::captureMouseCursor) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
        MouseScrolledEvent event(XOffset, YOffset);
        window.m_eventCallback(event);
    }
}

IMPLEMENT_GLFW_CALLBACK(CursorEnter, int Entered)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse || Input::captureMouseCursor) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
        CursorEnterEvent event(window, Entered);
        window.m_eventCallback(event);
    }
}

IMPLEMENT_GLFW_CALLBACK(CursorPos, double XPos, double YPos)
{
    static glm::vec2 last_position = {XPos, YPos};

    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse || Input::captureMouseCursor) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));

        Input::mouseInput.position = glm::vec2{XPos, YPos};
        Input::mouseInput.movement = Input::mouseInput.position - last_position;
        MouseMovedEvent event(XPos - Offset.x, YPos - Offset.y);
        window.m_eventCallback(event);
    }

    last_position = {XPos, YPos};
}

IMPLEMENT_GLFW_CALLBACK(MouseButton, int Button, int Action, int Mods)
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse || Input::captureMouseCursor) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
        switch (Action) {
            case GLFW_PRESS: {
                Input::mouseInput.buttons[Button].down = true;
                Input::mouseInput.buttons[Button].changed = true;
                MouseButtonPressedEvent event(static_cast<MouseCode>(Button));
                window.m_eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                Input::mouseInput.buttons[Button].down = false;
                Input::mouseInput.buttons[Button].changed = true;
                MouseButtonReleasedEvent event(static_cast<MouseCode>(Button));
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