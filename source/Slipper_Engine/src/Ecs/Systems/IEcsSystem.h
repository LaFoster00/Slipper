#pragma once

namespace Slipper
{
struct EcsSystemData
{
    std::function<void(entt::registry &)> executeFunction;
};

template<typename System> struct IEcsSystem
{
	virtual void Execute(entt::registry &Registry) = 0;

    static bool RegisterSystem()
    {
        EcsSystemData &data = Get().data;
        data.executeFunction = std::bind(&System::Execute, &Get(), std::placeholders::_1);
        return EcsInterface::AddSystem(data);
    }

    static System &Get()
    {
        static System system;
        return system;
    }

    EcsSystemData data;
    static inline bool systemAdded = RegisterSystem();
};
}  // namespace Slipper