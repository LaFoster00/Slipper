#pragma once

namespace Slipper
{
class AppComponent;
class Event;

template<typename T>
concept IsAppComponent = std::is_base_of_v<AppComponent, T>;

class AppComponent
{
 public:
    AppComponent(std::string_view Name);
    virtual ~AppComponent() = default;
    virtual void Init()
    {
	    OwningPtr<int> ptr_2;
        NonOwningPtr<const int> ptr_1 = ptr_2;
    	auto cast_ptr_1 = ptr_2.Cast<const int *>();
    };
    virtual void Shutdown(){};
    virtual void OnUpdate(){};
    virtual void OnGuiRender(){};
    virtual void OnEvent(Event &Event){};

    const std::string &GetName() const
    {
        return m_name;
    };

 protected:
    std::string m_name;
};
}  // namespace Slipper