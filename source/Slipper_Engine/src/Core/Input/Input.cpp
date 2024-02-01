#include "Input.h"

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

void InputManager::RegisterGlfwInputCallbacks(const Window &Window)
{
    glfwSetKeyCallback(Window, Key_Callback);
    glfwSetCursorPosCallback(Window, CursorPos_Callback);
    glfwSetMouseButtonCallback(Window, MouseButton_Callback);
    glfwSetScrollCallback(Window, Scroll_Callback);
    glfwSetWindowCloseCallback(Window, WindowClose_Callback);
    glfwSetWindowMaximizeCallback(Window, WindowMaximize_Callback);
    glfwSetWindowFocusCallback(Window, WindowFocus_Callback);
    glfwSetFramebufferSizeCallback(Window, FramebufferResize_Callback);
    glfwSetCursorEnterCallback(Window, CursorEnter_Callback);
}

void InputManager::SetInputOffset(glm::vec2 Offset)
{
    InputManager::Offset = Offset;
}

KeyCode InputManager::ImGuiToKeyCode(ImGuiKey Key)
{
    switch (Key) {
        case ImGuiKey_Tab:
            return KeyCode::Tab;
        case ImGuiKey_LeftArrow:
            return KeyCode::Left;
        case ImGuiKey_RightArrow:
            return KeyCode::Right;
        case ImGuiKey_UpArrow:
            return KeyCode::Up;
        case ImGuiKey_DownArrow:
            return KeyCode::Down;
        case ImGuiKey_PageUp:
            return KeyCode::PageUp;
        case ImGuiKey_PageDown:
            return KeyCode::PageDown;
        case ImGuiKey_Home:
            return KeyCode::Home;
        case ImGuiKey_End:
            return KeyCode::End;
        case ImGuiKey_Insert:
            return KeyCode::Insert;
        case ImGuiKey_Delete:
            return KeyCode::Delete;
        case ImGuiKey_Backspace:
            return KeyCode::Backspace;
        case ImGuiKey_Space:
            return KeyCode::Space;
        case ImGuiKey_Enter:
            return KeyCode::Enter;
        case ImGuiKey_Escape:
            return KeyCode::Escape;
        case ImGuiKey_Apostrophe:
            return KeyCode::Apostrophe;
        case ImGuiKey_Comma:
            return KeyCode::Comma;
        case ImGuiKey_Minus:
            return KeyCode::Minus;
        case ImGuiKey_Period:
            return KeyCode::Period;
        case ImGuiKey_Slash:
            return KeyCode::Slash;
        case ImGuiKey_Semicolon:
            return KeyCode::Semicolon;
        case ImGuiKey_Equal:
            return KeyCode::Equal;
        case ImGuiKey_LeftBracket:
            return KeyCode::LeftBracket;
        case ImGuiKey_Backslash:
            return KeyCode::Backslash;
        case ImGuiKey_RightBracket:
            return KeyCode::RightBracket;
        case ImGuiKey_GraveAccent:
            return KeyCode::GraveAccent;
        case ImGuiKey_CapsLock:
            return KeyCode::CapsLock;
        case ImGuiKey_ScrollLock:
            return KeyCode::ScrollLock;
        case ImGuiKey_NumLock:
            return KeyCode::NumLock;
        case ImGuiKey_PrintScreen:
            return KeyCode::PrintScreen;
        case ImGuiKey_Pause:
            return KeyCode::Pause;
        case ImGuiKey_Keypad0:
            return KeyCode::KP0;
        case ImGuiKey_Keypad1:
            return KeyCode::KP1;
        case ImGuiKey_Keypad2:
            return KeyCode::KP2;
        case ImGuiKey_Keypad3:
            return KeyCode::KP3;
        case ImGuiKey_Keypad4:
            return KeyCode::KP4;
        case ImGuiKey_Keypad5:
            return KeyCode::KP5;
        case ImGuiKey_Keypad6:
            return KeyCode::KP6;
        case ImGuiKey_Keypad7:
            return KeyCode::KP7;
        case ImGuiKey_Keypad8:
            return KeyCode::KP8;
        case ImGuiKey_Keypad9:
            return KeyCode::KP9;
        case ImGuiKey_KeypadDecimal:
            return KeyCode::KPDecimal;
        case ImGuiKey_KeypadDivide:
            return KeyCode::KPDivide;
        case ImGuiKey_KeypadMultiply:
            return KeyCode::KPMultiply;
        case ImGuiKey_KeypadSubtract:
            return KeyCode::KPSubtract;
        case ImGuiKey_KeypadAdd:
            return KeyCode::KPAdd;
        case ImGuiKey_KeypadEnter:
            return KeyCode::KPEnter;
        case ImGuiKey_KeypadEqual:
            return KeyCode::KPEqual;
        case ImGuiKey_LeftShift:
            return KeyCode::LeftShift;
        case ImGuiKey_LeftCtrl:
            return KeyCode::LeftControl;
        case ImGuiKey_LeftAlt:
            return KeyCode::LeftAlt;
        case ImGuiKey_LeftSuper:
            return KeyCode::LeftSuper;
        case ImGuiKey_RightShift:
            return KeyCode::RightShift;
        case ImGuiKey_RightCtrl:
            return KeyCode::RightControl;
        case ImGuiKey_RightAlt:
            return KeyCode::RightAlt;
        case ImGuiKey_RightSuper:
            return KeyCode::RightSuper;
        case ImGuiKey_Menu:
            return KeyCode::Menu;
        case ImGuiKey_0:
            return KeyCode::D0;
        case ImGuiKey_1:
            return KeyCode::D1;
        case ImGuiKey_2:
            return KeyCode::D2;
        case ImGuiKey_3:
            return KeyCode::D3;
        case ImGuiKey_4:
            return KeyCode::D4;
        case ImGuiKey_5:
            return KeyCode::D5;
        case ImGuiKey_6:
            return KeyCode::D6;
        case ImGuiKey_7:
            return KeyCode::D7;
        case ImGuiKey_8:
            return KeyCode::D8;
        case ImGuiKey_9:
            return KeyCode::D9;
        case ImGuiKey_A:
            return KeyCode::A;
        case ImGuiKey_B:
            return KeyCode::B;
        case ImGuiKey_C:
            return KeyCode::C;
        case ImGuiKey_D:
            return KeyCode::D;
        case ImGuiKey_E:
            return KeyCode::E;
        case ImGuiKey_F:
            return KeyCode::F;
        case ImGuiKey_G:
            return KeyCode::G;
        case ImGuiKey_H:
            return KeyCode::H;
        case ImGuiKey_I:
            return KeyCode::I;
        case ImGuiKey_J:
            return KeyCode::J;
        case ImGuiKey_K:
            return KeyCode::K;
        case ImGuiKey_L:
            return KeyCode::L;
        case ImGuiKey_M:
            return KeyCode::M;
        case ImGuiKey_N:
            return KeyCode::N;
        case ImGuiKey_O:
            return KeyCode::O;
        case ImGuiKey_P:
            return KeyCode::P;
        case ImGuiKey_Q:
            return KeyCode::Q;
        case ImGuiKey_R:
            return KeyCode::R;
        case ImGuiKey_S:
            return KeyCode::S;
        case ImGuiKey_T:
            return KeyCode::T;
        case ImGuiKey_U:
            return KeyCode::U;
        case ImGuiKey_V:
            return KeyCode::V;
        case ImGuiKey_W:
            return KeyCode::W;
        case ImGuiKey_X:
            return KeyCode::X;
        case ImGuiKey_Y:
            return KeyCode::Y;
        case ImGuiKey_Z:
            return KeyCode::Z;
        case ImGuiKey_F1:
            return KeyCode::F1;
        case ImGuiKey_F2:
            return KeyCode::F2;
        case ImGuiKey_F3:
            return KeyCode::F3;
        case ImGuiKey_F4:
            return KeyCode::F4;
        case ImGuiKey_F5:
            return KeyCode::F5;
        case ImGuiKey_F6:
            return KeyCode::F6;
        case ImGuiKey_F7:
            return KeyCode::F7;
        case ImGuiKey_F8:
            return KeyCode::F8;
        case ImGuiKey_F9:
            return KeyCode::F9;
        case ImGuiKey_F10:
            return KeyCode::F10;
        case ImGuiKey_F11:
            return KeyCode::F11;
        case ImGuiKey_F12:
            return KeyCode::F12;
        default:
            return KeyCode::None;
    }
}

IMPLEMENT_INPUT_CALLBACK(FramebufferResize, int Width, int Height)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));

    Application::Get().OnWindowResize(&window, Width, Height);

    window.m_info.width = Width;
    window.m_info.height = Height;
    auto io = ImGui::GetIO();
}

IMPLEMENT_INPUT_CALLBACK(Key, int Key, int Scancode, int Action, int Mods)
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

IMPLEMENT_INPUT_CALLBACK(Scroll, double XOffset, double YOffset)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse || Input::captureMouseCursor) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
        MouseScrolledEvent event(XOffset, YOffset);
        window.m_eventCallback(event);
    }
}

IMPLEMENT_INPUT_CALLBACK(CursorEnter, int Entered)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse || Input::captureMouseCursor) {
        Slipper::Window &window = *static_cast<Slipper::Window *>(
            glfwGetWindowUserPointer(Window));
        CursorEnterEvent event(window, Entered);
        window.m_eventCallback(event);
    }
}

IMPLEMENT_INPUT_CALLBACK(CursorPos, double XPos, double YPos)
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

IMPLEMENT_INPUT_CALLBACK(MouseButton, int Button, int Action, int Mods)
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

IMPLEMENT_INPUT_CALLBACK(WindowMaximize, int Maximized)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    WindowMaximizeEvent event(window, Maximized);
    window.m_eventCallback(event);
}

IMPLEMENT_INPUT_CALLBACK(WindowFocus, int Focused)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    WindowFocusEvent event(window, Focused);
    window.m_eventCallback(event);
}

IMPLEMENT_INPUT_DIRECT_CALLBACK(WindowClose)
{
    Slipper::Window &window = *static_cast<Slipper::Window *>(glfwGetWindowUserPointer(Window));
    WindowCloseEvent event(window);
    window.m_eventCallback(event);
}
}  // namespace Slipper