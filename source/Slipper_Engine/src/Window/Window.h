#pragma once

#include <functional>
#include <unordered_map>

#include "Engine.h"

struct WindowInfo
{
 public:
    const char *name;
    uint32_t width;
    uint32_t height;
    bool resizable;
};

class Window
{
 public:
    Window(WindowInfo CreateInfo);
    Window(Window &Window) = delete;
    Window(Window &&Window) = delete;
    ~Window()
    {
        glfwDestroyWindow(glfwWindow);
    }

    bool ShouldClose() const
    {
        return glfwWindowShouldClose(glfwWindow);
    }

    operator GLFWwindow *() const
    {
        return glfwWindow;
    }

    glm::vec2 GetSize() const;

    void AddResizeCallback(void *Context, std::function<void(Window &, int, int)> Callback);
    void RemoveResizeCallback(void *Context, std::function<void(Window &, int, int)> Callback);

 private:
    static void FramebufferResizeCallback(GLFWwindow *Window, int Width, int Height);

 public:
    GLFWwindow *glfwWindow;

 private:
    WindowInfo m_info;
    static std::unordered_map<GLFWwindow *, Window *> m_windows;
    std::unordered_map<void *, std::function<void(Window &, int, int)>> m_resizeCallbacks;
};