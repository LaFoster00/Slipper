#pragma once
#include <sstream>

#include "Event.h"
#include "Window.h"

namespace Slipper
{
class WindowEvent : public Event
{
 public:
    EVENT_CLASS_CATEGORY(EventCategoryWindow)

    Window &window;

 protected:
    WindowEvent(Window &Window) : window(Window)
    {
    }
};

class WindowResizeEvent : public WindowEvent
{
 public:
    WindowResizeEvent(Window &Window, uint32_t Width, uint32_t Height)
        : WindowEvent(Window), width(Width), height(Height)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << window.GetName() << " [ " << width << " , " << height
           << " ]";
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowResized)

    const uint32_t width, height;
};

class WindowMaximizeEvent : public WindowEvent
{
 public:
    WindowMaximizeEvent(Window &Window, bool Maximized) : WindowEvent(Window), maximized(Maximized)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowMaximizeEvent: " << window.GetName() << ' ' << maximized;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowMaximized)
    const bool maximized;
};

class WindowFocusEvent : public WindowEvent
{
 public:
    WindowFocusEvent(Window &Window, bool Focused) : WindowEvent(Window), focused(Focused)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowFocusEvent: " << window.GetName() << ' ' << focused;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowFocus)
    const bool focused;
};

class WindowCloseEvent : public WindowEvent
{
 public:
    WindowCloseEvent(Window &Window) : WindowEvent(Window)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowCloseEvent: " << window.GetName();
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowClose)
};

class CursorEnterEvent : public WindowEvent
{
 public:
    CursorEnterEvent(Window &Window, bool Entered) : WindowEvent(Window), entered(Entered)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "CursorEnterEvent: " << window.GetName() << " " << entered;
        return ss.str();
    }

    EVENT_CLASS_TYPE(CursorEnter)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

    const bool entered;
};
}  // namespace Slipper
