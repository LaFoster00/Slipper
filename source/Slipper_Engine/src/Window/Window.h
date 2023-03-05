#pragma once

#define DECLARE_GLFW_DIRECT_CALLBACK(FnName) static void FnName##_Callback(GLFWwindow *Window)

#define DECLARE_GLFW_CALLBACK(FnName, ...) \
    static void FnName##_Callback(GLFWwindow *Window, __VA_ARGS__)

#define IMPLEMENT_GLFW_DIRECT_CALLBACK(FnName) void Window::FnName##_Callback(GLFWwindow *Window)

#define IMPLEMENT_GLFW_CALLBACK(FnName, ...) \
    void Window::FnName##_Callback(GLFWwindow *Window, __VA_ARGS__)

namespace Slipper
{
class WindowEvent;
class Surface;
class Event;
using EventCallback = std::function<void(Event &)>;

struct WindowInfo
{
    std::string name;
    uint32_t width;
    uint32_t height;
    bool resizable;
};

class Window
{
    friend WindowEvent;

 public:
    Window(WindowInfo CreateInfo);
    Window(Window &Window) = delete;
    Window(Window &&Window) = delete;
    ~Window();

    void SetTitle(std::string_view Name);

    Surface &GetSurface() const
    {
        return *m_surface;
    }

    operator GLFWwindow *() const
    {
        return glfwWindow;
    }

    void SetEventCallback(EventCallback Callback)
    {
        m_eventCallback = Callback;
    }

    VkExtent2D GetSize() const
    {
        return {m_info.width, m_info.height};
    }

    std::string_view GetName() const
    {
        return m_info.name;
    }

    void OnUpdate();

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

 public:
    GLFWwindow *glfwWindow;

 private:
    WindowInfo m_info;
    std::unique_ptr<Surface> m_surface;
    EventCallback m_eventCallback;
};
}  // namespace Slipper