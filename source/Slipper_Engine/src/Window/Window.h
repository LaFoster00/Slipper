#pragma once

namespace Slipper
{
class InputManager;
class WindowEvent;
class Context;
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
    friend InputManager;

 public:
    Window(WindowInfo CreateInfo);
    Window(Window &Window) = delete;
    Window(Window &&Window) = delete;
    ~Window();

    void SetTitle(std::string_view Name);

    Context &GetContext() const
    {
        return *m_context;
    }

    operator GLFWwindow *() const
    {
        return glfwWindow;
    }

    void SetEventCallback(const EventCallback Callback)
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

    static void OnUpdate();

 public:
    GLFWwindow *glfwWindow;

 private:
    WindowInfo m_info;
    std::unique_ptr<Context> m_context;
    EventCallback m_eventCallback;
};
}  // namespace Slipper