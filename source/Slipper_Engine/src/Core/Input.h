#pragma once
#include <cstdint>

#include "AppComponent.h"

namespace Slipper
{
using MouseCodeT = uint16_t;

enum class MouseCode : MouseCodeT
{
    // From glfw3.h
    Button0 = 0,
    Button1 = 1,
    Button2 = 2,
    Button3 = 3,
    Button4 = 4,
    Button5 = 5,
    Button6 = 6,
    Button7 = 7,

    ButtonLast = Button7,
    ButtonLeft = Button0,
    ButtonRight = Button1,
    ButtonMiddle = Button2,

    None = 8,
    ArraySize = 9
};

using KeyCodeT = uint16_t;

enum class KeyCode : KeyCodeT
{
    // From glfw3.h
    Space = 32,
    Apostrophe = 39, /* ' */
    Comma = 44,      /* , */
    Minus = 45,      /* - */
    Period = 46,     /* . */
    Slash = 47,      /* / */

    D0 = 48, /* 0 */
    D1 = 49, /* 1 */
    D2 = 50, /* 2 */
    D3 = 51, /* 3 */
    D4 = 52, /* 4 */
    D5 = 53, /* 5 */
    D6 = 54, /* 6 */
    D7 = 55, /* 7 */
    D8 = 56, /* 8 */
    D9 = 57, /* 9 */

    Semicolon = 59, /* ; */
    Equal = 61,     /* = */

    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,

    LeftBracket = 91,  /* [ */
    Backslash = 92,    /* \ */
    RightBracket = 93, /* ] */
    GraveAccent = 96,  /* ` */

    World1 = 161, /* non-US #1 */
    World2 = 162, /* non-US #2 */

    /* Function keys */
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,

    /* Keypad */
    KP0 = 320,
    KP1 = 321,
    KP2 = 322,
    KP3 = 323,
    KP4 = 324,
    KP5 = 325,
    KP6 = 326,
    KP7 = 327,
    KP8 = 328,
    KP9 = 329,
    KPDecimal = 330,
    KPDivide = 331,
    KPMultiply = 332,
    KPSubtract = 333,
    KPAdd = 334,
    KPEnter = 335,
    KPEqual = 336,

    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348,

    ArraySize = 349
};

#define DECLARE_GLFW_DIRECT_CALLBACK(FnName) static void FnName##_Callback(GLFWwindow *Window)

#define DECLARE_GLFW_CALLBACK(FnName, ...) \
    static void FnName##_Callback(GLFWwindow *Window, __VA_ARGS__)

#define IMPLEMENT_GLFW_DIRECT_CALLBACK(FnName) \
    void InputManager::FnName##_Callback(GLFWwindow *Window)

#define IMPLEMENT_GLFW_CALLBACK(FnName, ...) \
    void InputManager::FnName##_Callback(GLFWwindow *Window, __VA_ARGS__)

struct MouseInput
{
    struct MouseButtonInput
    {
        MouseCode button = MouseCode::None;
        bool down = false;
        bool changed = false;
    };
    std::array<MouseButtonInput, static_cast<size_t>(MouseCode::ArraySize)> buttons;

    glm::vec2 movement = {};
};

struct KeyInput
{
    bool down = false;
    bool changed = false;
};

class InputManager;
class Application;

class Input
{
    friend InputManager;
    friend Application;

 public:
    static bool GetMouseButtonDown(MouseCode Button);
    static bool GetMouseButtonReleased(MouseCode Button);
    static bool GetMouseButtonPressed(MouseCode Button);

    static void CaptureMouse(bool Capture);
    static glm::vec2 GetMouseMovement();

    static bool GetKeyDown(KeyCode Key);
    static bool GetKeyReleased(KeyCode Key);
    static bool GetKeyPressed(KeyCode Key);

 private:
    static void UpdateInputs();

 public:
    static inline bool enteredWindow;
    static inline bool exitedWindow;
    static inline bool insideWindow;
    static inline MouseInput mouseInput;
    static inline std::array<KeyInput, static_cast<std::size_t>(KeyCode::ArraySize)> keyInputs;

    static inline bool captureMouseCursor;
};

class InputManager : public AppComponent
{
    friend Input;

 public:
    InputManager() : AppComponent("Input Manager")
    {
    }

    static void RegisterInputCallbacks(const Window &Window);
    static void SetInputOffset(glm::vec2 Offset);

 private:
    DECLARE_GLFW_CALLBACK(FramebufferResize, int Width, int Height);
    DECLARE_GLFW_CALLBACK(Key, int Key, int Scancode, int Action, int Mods);
    DECLARE_GLFW_CALLBACK(Scroll, double XOffset, double YOffset);
    DECLARE_GLFW_CALLBACK(CursorEnter, int Entered);
    DECLARE_GLFW_CALLBACK(CursorPos, double XPos, double YPos);
    DECLARE_GLFW_CALLBACK(MouseButton, int Button, int Action, int Mods);
    DECLARE_GLFW_CALLBACK(WindowMaximize, int Maximized);
    DECLARE_GLFW_CALLBACK(WindowFocus, int Focused);
    DECLARE_GLFW_DIRECT_CALLBACK(WindowClose);

 private:
    static inline glm::vec2 Offset = {};
};

}  // namespace Slipper