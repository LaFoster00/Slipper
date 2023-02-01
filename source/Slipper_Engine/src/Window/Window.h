#pragma once

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

 private:
    static void FramebufferResizeCallback(GLFWwindow *Window, int Width, int Height);

 public:
    GLFWwindow *glfwWindow;

 private:
    WindowInfo m_info;
};