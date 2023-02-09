#pragma once
#include <string_view>

namespace Slipper
{
class ProgramComponent
{
 public:
    ProgramComponent(const std::string_view Name = "ProgramComponent");
    virtual ~ProgramComponent() = default;
    virtual void Init(){};
    virtual void Shutdown(){};
    virtual void OnUpdate(){};
    virtual void OnGuiRender(){};

    const std::string &GetName() const
    {
        return m_name;
    };

 protected:
    std::string m_name;
};
}  // namespace Slipper