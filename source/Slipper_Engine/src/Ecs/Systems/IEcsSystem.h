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

    static inline EcsSystemData data;
    static inline bool systemAdded = EcsInterface::AddSystem(data);
};
}  // namespace Slipper