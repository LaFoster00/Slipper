#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "Engine.h"
#include "Presentation/Surface.h"

namespace Slipper
{
struct WindowInfo
{
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
        m_surface.reset();
        glfwDestroyWindow(glfwWindow);
    }

    Surface &GetSurface() const
    {
        return *m_surface;
    }

    operator GLFWwindow *() const
    {
        return glfwWindow;
    }

    glm::vec2 GetSize() const;

    void OnUpdate();

 private:
    static void FramebufferResizeCallback(GLFWwindow *Window, int Width, int Height);

 public:
    GLFWwindow *glfwWindow;

 private:
    WindowInfo m_info;
    std::unique_ptr<Surface> m_surface;

    static std::unordered_map<GLFWwindow *, Window *> m_windows;
};
}  // namespace Slipper