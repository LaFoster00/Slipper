#pragma once
#include "EditorAppComponent.h"
#include "Vulkan/vk_DeviceDependentObject.h"

namespace Slipper
{
    class ComputeShader;
    class UniformBuffer;
    class Buffer;
}  // namespace Slipper

namespace Slipper::Editor
{
    class ComputeShaderTest : public EditorAppComponent, GPU::Vulkan::DeviceDependentObject
    {
     private:
        struct Particle
        {
            glm::vec2 position;
            glm::vec2 velocity;
            glm::vec4 color;
        };

        struct alignas(16) DeltaTime
        {
            float deltaTime;
        };

     public:
        ComputeShaderTest();

        void Init() override;
        void Shutdown() override;
        void OnUpdate() override;
        ~ComputeShaderTest() override;

     private:
        uint32_t m_particleCount = 1024;
        VkDeviceSize m_bufferSize;
        std::vector<OwningPtr<Buffer>> m_storageBuffers;
        NonOwningPtr<ComputeShader> m_computeShader;
    };
}  // namespace Slipper::Editor
