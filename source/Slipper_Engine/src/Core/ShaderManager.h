#pragma once

#include "Shader/Shader.h"

namespace Slipper
{
class ShaderManager
{
 public:
    static Slipper::Shader *GetShader(const std::string &filepath);
    static void ShutDown();

 private:
    static std::unordered_map<std::string, Slipper::Shader *> m_shader;
};
}  // namespace Slipper