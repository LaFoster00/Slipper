#include "ComputeShaderTest.h"

#include "Buffer/Buffer.h"
#include "ShaderManager.h"

namespace Slipper::Editor
{
ComputeShaderTest::ComputeShaderTest()
    : EditorAppComponent("Compute Shader Test"), m_bufferSize(m_particleCount * sizeof(Particle))
{
}

void ComputeShaderTest::Init()
{
    EditorAppComponent::Init();

    m_storageBuffers.reserve(Engine::MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; ++i) {
        m_storageBuffers.emplace_back(new Buffer(
            m_bufferSize,
            static_cast<VkBufferUsageFlags>(vk::BufferUsageFlagBits::eVertexBuffer |
                                            vk::BufferUsageFlagBits::eStorageBuffer |
                                            vk::BufferUsageFlagBits::eTransferDst),
            static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eDeviceLocal)));
    }

    m_computeShader = ShaderManager::LoadShader(
        {"./EditorContent/Shaders/Spir-V/ParticleTest.comp.spv"});

    // Initialize particles
    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

    // Initial particle positions on a circle
    std::vector<Particle> particles(m_particleCount);
    for (auto &particle : particles) {
        float r = 0.25f * sqrt(rndDist(rndEngine));
        float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
        float x = r * cos(theta) * 1920 / 1080;
        float y = r * sin(theta);
        particle.position = glm::vec2(x, y);
        particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
        particle.color = glm::vec4(
            rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
    }

    Buffer particle_staging_buffer(
        m_bufferSize,
        static_cast<VkBufferUsageFlags>(vk::BufferUsageFlagBits::eTransferSrc),
        static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eHostVisible |
                                           vk::MemoryPropertyFlagBits::eHostCoherent));

    for (const auto &storage_buffer : m_storageBuffers) {
        storage_buffer->CopyBuffer(particle_staging_buffer);
    }

    for (uint32_t i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; ++i) {
        m_computeShader->BindShaderUniform("pIn", *m_storageBuffers[i], i);
        m_computeShader->BindShaderUniform(
            "pOut", *m_storageBuffers[Engine::MAX_FRAMES_IN_FLIGHT - 1 - i], i);
    }
}

void ComputeShaderTest::Shutdown()
{
    EditorAppComponent::Shutdown();
}

void ComputeShaderTest::OnUpdate()
{
    EditorAppComponent::OnUpdate();
}

ComputeShaderTest::~ComputeShaderTest()
{
}
}  // namespace Slipper::Editor
