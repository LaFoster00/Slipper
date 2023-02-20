#pragma once

namespace Slipper
{
class AppComponent
{
 public:
    AppComponent(std::string_view Name = "AppComponent");
    virtual ~AppComponent() = default;
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