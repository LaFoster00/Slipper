#pragma once
#include "Object.h"

namespace Slipper
{
    class Window;
}

namespace Slipper::GPU
{
    class Context : Object<Context>
    {
     public:
        static Context *CreateContext(Window &window);

        virtual void WaitIdle() = 0;
        virtual Engine::GpuBackend BackendType() = 0;
    };
}  // namespace Slipper::GPU
