#include "ShaderManager.h"

#include "Vulkan/vk_ComputeShader.h"
#include "Vulkan/vk_GraphicsShader.h"
#include "Vulkan/vk_Shader.h"


namespace Slipper
{
    namespace GPU::Vulkan
    {
        enum class ShaderType;
    }

    NonOwningPtr<GPU::Vulkan::Shader> ShaderManager::GetShader(const std::string_view &Name)
    {
        if (const auto hash = StringViewHash{}(Name); m_namedShaders.contains(hash))
        {
            return m_namedShaders.at(hash);
        }
        LOG_FORMAT("Shader '{}' does not exist!", Name)
        return {};
    }

    NonOwningPtr<GPU::Vulkan::GraphicsShader> ShaderManager::TryGetGraphicsShader(const std::string_view Name)
    {
        if (auto shader = GetShader(Name))
        {
            return shader.TryCast<GPU::Vulkan::GraphicsShader>();
        }
        return nullptr;
    }

    NonOwningPtr<GPU::Vulkan::ComputeShader> ShaderManager::TryGetComputeShader(const std::string_view Name)
    {
        if (auto shader = GetShader(Name))
        {
            return shader.TryCast<GPU::Vulkan::ComputeShader>();
        }
        return nullptr;
    }

    NonOwningPtr<GPU::Vulkan::GraphicsShader> ShaderManager::LoadGraphicsShader(
        const std::vector<std::string_view> &Filepaths)
    {
        const auto shader_name = File::remove_file_type_from_name(File::get_file_name_from_path(Filepaths[0]));
        const auto hash = StringViewHash{}(shader_name);
        if (m_namedShaders.contains(hash))
        {
            if (auto shader = m_namedShaders.at(hash); shader.TryCast<GPU::Vulkan::GraphicsShader>())
            {
                LOG_FORMAT("Returned already loaded shader '{}'", shader_name)
                return shader.TryCast<GPU::Vulkan::GraphicsShader>();
            }
            else
            {
                LOG_FORMAT("Returned null. Allready loaded shader '{}' is not a graphics shader.", shader_name)
                return nullptr;
            }
        }

        std::vector<std::tuple<std::string_view, GPU::Vulkan::ShaderType>> shader_stages;
        for (auto &filepath : Filepaths)
        {
            auto file_ending = File::get_shader_type_from_spirv_path(filepath);
            if (file_ending == "vert")
                shader_stages.emplace_back(filepath, GPU::Vulkan::ShaderType::VERTEX);
            else if (file_ending == "frag")
                shader_stages.emplace_back(filepath, GPU::Vulkan::ShaderType::FRAGMENT);
            else if (file_ending == "comp")
                ASSERT(false, "If you want to load Compute Shaders use LoadComputeShader instead!")
        }
        const auto &new_shader = m_graphicsShaders.emplace_back(new GPU::Vulkan::GraphicsShader(shader_stages));
        if (shader_stages.size() == 1)
        {
            LOG_FORMAT("Loaded {} shader '{}'",
                       GPU::Vulkan::ShaderTypeNames[static_cast<int>(std::get<1>(shader_stages[0]))],
                       shader_name);
        }
        else
        {
            LOG_FORMAT("Loaded shader '{}' with shader stages: ", shader_name);
            for (auto &[name, type] : shader_stages)
            {
                LOG_FORMAT("\t{}", GPU::Vulkan::ShaderTypeNames[static_cast<int>(type)])
            }
        }

        m_namedShaders.emplace(hash, NonOwningPtr<GPU::Vulkan::Shader>(new_shader)).first->second;
        return new_shader;
    }

    NonOwningPtr<GPU::Vulkan::ComputeShader> ShaderManager::LoadComputeShader(const std::string_view Filepath)
    {
        const auto shader_name = File::remove_file_type_from_name(File::get_file_name_from_path(Filepath));
        const auto hash = StringViewHash{}(shader_name);
        if (m_namedShaders.contains(hash))
        {
            if (auto shader = m_namedShaders.at(hash); shader.TryCast<GPU::Vulkan::ComputeShader>())
            {
                LOG_FORMAT("Returned already loaded shader '{}'", shader_name)
                return shader.TryCast<GPU::Vulkan::ComputeShader>();
            }
            else
            {
                LOG_FORMAT("Returned null. Allready loaded shader '{}' is not a compute shader.", shader_name)
                return nullptr;
            }
        }

        auto file_ending = File::get_shader_type_from_spirv_path(Filepath);
        if (file_ending != "comp")
            ASSERT(false, "If you want to load Graphics Shaders use LoadGraphicsShader instead!")

        const auto &new_shader = m_computeShaders.emplace_back(new GPU::Vulkan::ComputeShader(Filepath));
        LOG_FORMAT("Loaded Compute shader '{}'", shader_name);
        m_namedShaders.emplace(hash, NonOwningPtr<GPU::Vulkan::Shader>(new_shader)).first->second;
        return new_shader;
    }

    void ShaderManager::Shutdown()
    {
        m_namedShaders.clear();
        m_graphicsShaders.clear();
        m_computeShaders.clear();
    }
}  // namespace Slipper
