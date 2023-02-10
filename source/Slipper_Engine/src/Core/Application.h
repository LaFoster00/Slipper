#pragma once
#include <string>
#include <vector>

#include "common_defines.h"

int main(int argc, char *argv[]);

namespace Slipper
{
class Gui;
class VulkanInstance;
class Window;
class GraphicsEngine;

class AppComponent;

struct ApplicationInfo
{
    std::string Name = "Slipper Engine";
};

// Program witch will start the render engine and open a window with ability to add more
// functionality through ProgramComponents
class Application
{
 public:
    Application(ApplicationInfo &ApplicationInfo);
    virtual ~Application();

    void AddComponent(AppComponent *ProgramComponent);

    void Close();
    static Application &Get()
    {
        return *instance;
    }

    virtual void Run();
    virtual void OnWindowResize(Window *Window, int Width, int Height);

 public:
    std::unique_ptr<Window> window;

 protected:
    static Application *instance;

    std::string name;
    bool running = true;
    bool minimized = false;

    std::unique_ptr<VulkanInstance> vulkanInstance;

    std::unique_ptr<Gui> guiComponent;
    std::vector<std::unique_ptr<AppComponent>> appComponents;
};
}  // namespace Slipper