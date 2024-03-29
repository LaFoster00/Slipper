#pragma once
#include "AppComponent.h"

int main(int argc, char *argv[]);

namespace Slipper
{
    class MaterialManager;
    class Ecs;

    namespace GPU
    {
        namespace Vulkan
        {
            class VulkanInstance;
        }

        class RenderingStage;
    }

    class Window;

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
        Application();
        virtual ~Application() = default;
        virtual void Init(ApplicationInfo &ApplicationInfo);
        virtual void Shutdown();

        template<typename T>
            requires IsAppComponent<T>
        NonOwningPtr<T> AddComponent(NonOwningPtr<T> ProgramComponent)
        {
            ProgramComponent->Init();
            appComponentsOrdered.emplace_back(ProgramComponent);
            return static_cast<T *>(appComponents.emplace_back(ProgramComponent).get());
        }

        template<typename T>
            requires IsAppComponent<T>
        NonOwningPtr<T> AddComponent(T *ProgramComponent)
        {
            ProgramComponent->Init();
            appComponentsOrdered.emplace_back(ProgramComponent);
            return static_cast<T *>(appComponents.emplace_back(ProgramComponent).get());
        }

        template<typename T, typename U>
            requires IsAppComponent<T> && IsAppComponent<U>
        NonOwningPtr<T> AddComponentBefore(T *ProgramComponent, NonOwningPtr<U> ExecuteBefore)
        {
            auto execute_before_itr = std::find(
                appComponentsOrdered.begin(), appComponentsOrdered.end(), ExecuteBefore);
            if (execute_before_itr != appComponentsOrdered.end())
            {
                appComponentsOrdered.emplace(execute_before_itr, ProgramComponent);
            }

            ProgramComponent->Init();
            return static_cast<T *>(appComponents.emplace_back(ProgramComponent).get());
        }

        template<typename T, typename U>
            requires IsAppComponent<T> && IsAppComponent<U>
        NonOwningPtr<T> AddComponentAfter(T *ProgramComponent, NonOwningPtr<U> ExecuteAfter)
        {
            auto execute_after_itr = std::find(appComponentsOrdered.begin(), appComponentsOrdered.end(), ExecuteAfter);
            std::advance(execute_after_itr, 1);
            if (execute_after_itr != appComponentsOrdered.end())
            {
                appComponentsOrdered.emplace(execute_after_itr, ProgramComponent);
            }

            ProgramComponent->Init();
            return static_cast<T *>(appComponents.emplace_back(ProgramComponent).get());
        }

        void Close();
        void CloseWindow(const Window *Window);

        static Application &Get()
        {
            return *instance;
        }

        template<typename T>
        T &Get()
        {
            return *dynamic_cast<T *>(&Get());
        }

        virtual void Run();
        virtual void OnEvent(Event &Event);
        virtual void OnWindowResize(Window *Window, int Width, int Height);
        virtual void OnViewportResize(NonOwningPtr<GPU::RenderingStage> Stage, uint32_t Width, uint32_t Height);
        void AddViewportResizeCallback(std::function<void(NonOwningPtr<GPU::RenderingStage>, uint32_t, uint32_t)> Callback);
        void AddAdditionalRenderStageUpdate(NonOwningPtr<GPU::RenderingStage> Stage,
                                            std::function<void(NonOwningPtr<GPU::RenderingStage>)> UpdateFunction);
        void AddAdditionalRenderStageUpdateFront(NonOwningPtr<GPU::RenderingStage> Stage,
                                                 std::function<void(NonOwningPtr<GPU::RenderingStage>)> UpdateFunction);

     private:
        void ViewportResize(NonOwningPtr<GPU::RenderingStage> Stage);

     public:
        OwningPtr<Window> window;

     protected:
        static inline Application *instance = nullptr;

        std::string name;
        bool running = true;
        bool minimized = false;

        ResizeInfo windowResize;
        std::unordered_map<NonOwningPtr<GPU::RenderingStage>, ResizeInfo> viewportsResize;
        std::vector<std::function<void(NonOwningPtr<GPU::RenderingStage>, uint32_t, uint32_t)>> viewportResizeCallbacks;

        OwningPtr<GPU::Vulkan::VulkanInstance> vulkanInstance;

        NonOwningPtr<Ecs> ecsComponent;
        NonOwningPtr<MaterialManager> materialManager;
        std::vector<OwningPtr<AppComponent>> appComponents;
        std::vector<NonOwningPtr<AppComponent>> appComponentsOrdered;

        std::unordered_map<NonOwningPtr<GPU::RenderingStage>, std::vector<std::function<void(NonOwningPtr<GPU::RenderingStage>)>>>
            renderingStagesUpdates;
    };
}  // namespace Slipper
