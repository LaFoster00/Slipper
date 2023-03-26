#pragma once
#include "AppComponent.h"

int main(int argc, char *argv[]);

namespace Slipper
{
class RenderingStage;
class Event;
class Gui;
class Ecs;
class VulkanInstance;
class Window;
class GraphicsEngine;

struct ApplicationInfo
{
    std::string Name = "Slipper Engine";
};

struct ResizeInfo
{
    std::any context;
    bool resized = false;
    uint32_t width = 0, height = 0;
};

// Program witch will start the render engine and open a window with ability to add more
// functionality through ProgramComponents
class Application
{
 public:
    Application(ApplicationInfo &ApplicationInfo);
    virtual ~Application();

    virtual void Init();

    template<typename T>
        requires IsAppComponent<T>
    NonOwningPtr<T> AddComponent(NonOwningPtr<T> ProgramComponent)
    {
        ProgramComponent->Init();
        return static_cast<T *>(appComponents.emplace_back(ProgramComponent).get());
    }

    template<typename T>
        requires IsAppComponent<T>
    NonOwningPtr<T> AddComponent(T *ProgramComponent)
    {
        ProgramComponent->Init();
        return static_cast<T *>(appComponents.emplace_back(ProgramComponent).get());
    }

    void Close();
    void CloseWindow(const Window *Window);

    static Application &Get()
    {
        return *instance;
    }

    virtual void Run();
    virtual void OnEvent(Event &Event);
    virtual void OnWindowResize(Window *Window, int Width, int Height);
    virtual void OnViewportResize(uint32_t Width, uint32_t Height);
    void AddViewportResizeCallback(std::function<void(uint32_t, uint32_t)> Callback);
    void AddAdditionalRenderStageUpdate(NonOwningPtr<RenderingStage> Stage, std::function<void(NonOwningPtr<RenderingStage>)> UpdateFunction);

 private:
    void WindowResize();
    void ViewportResize();

 public:
    std::unique_ptr<Window> window;

 protected:
    static Application *instance;

    std::string name;
    bool running = true;
    bool minimized = false;

    ResizeInfo windowResize;
    ResizeInfo viewportResize;
    std::vector<std::function<void(uint32_t, uint32_t)>> viewportResizeCallbacks;

    std::unique_ptr<VulkanInstance> vulkanInstance;

    NonOwningPtr<Ecs> ecsComponent;
    std::vector<OwningPtr<AppComponent>> appComponents;

    std::unordered_map<NonOwningPtr<RenderingStage>,
                       std::vector<std::function<void(NonOwningPtr<RenderingStage>)>>>
        renderingStagesUpdate;
};
}  // namespace Slipper