#pragma once
#include <string>

#include "common_defines.h"

EXTERNC
{
    SLIPPER_FUNCTION int main(int argc, char *argv[]);

    namespace Slipper
    {
        class VulkanInstance;
        class Window;
    class GraphicsEngine;

    class ProgramComponent;

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

        void AddProgramComponent(ProgramComponent *ProgramComponent);

        void Close();
        static Application &Get()
        {
            return *instance;
        }

        void Run();

     protected:
        static Application *instance;

        std::string name;
        bool running = true;

        std::unique_ptr<VulkanInstance> vulkanInstance;
        std::unique_ptr<Window> window;
    };
    }  // namespace Slipper
}
