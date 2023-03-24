#pragma once
#include "Event.h"
#include "Input.h"

namespace Slipper
{
class MouseMovedEvent : public Event
{
 public:
    MouseMovedEvent(const float x, const float y) : mouseX(x), mouseY(y)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseMovedEvent: " << mouseX << ", " << mouseY;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    float mouseX, mouseY;
};

class MouseScrolledEvent : public Event
{
 public:
    MouseScrolledEvent(const double xOffset, const double yOffset)
        : xOffset(xOffset), yOffset(yOffset)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseScrolledEvent: " << xOffset << ", " << yOffset;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    double xOffset, yOffset;
};

class MouseButtonEvent : public Event
{
 public:
    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonEvent: " << button;
        return ss.str();
    }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
 protected:
    MouseButtonEvent(const MouseCode button) : button(button)
    {
    }

 public:
    MouseCode button;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
 public:
    MouseButtonPressedEvent(MouseCode button) : MouseButtonEvent(button)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonPressedEvent: " << button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
 public:
    MouseButtonReleasedEvent(MouseCode button) : MouseButtonEvent(button)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonReleasedEvent: " << button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

// Keyboard Inputs
class KeyEvent : public Event
{
 public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

 protected:
    KeyEvent(KeyCode Key) : key(Key)
    {
    }

 public:
    const KeyCode key;
};

class KeyPressedEvent : public KeyEvent
{
 public:
    KeyPressedEvent(KeyCode Key, bool Repeated) : KeyEvent(Key), repeated(Repeated)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << key;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)

 public:
    bool repeated;
};

class KeyReleasedEvent : public KeyEvent
{
 public:
    KeyReleasedEvent(KeyCode Key) : KeyEvent(Key)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << key;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};
}  // namespace Slipper
