#pragma once
#include "Object.h"

namespace Slipper::GPU
{
    class CommandPool : public Object<CommandPool>
    {
    public:
        CommandPool() = delete;
        static CommandPool *Create();
        virtual VkCommandBuffer GetCurrentCommandBuffer() = 0;
    };
}
