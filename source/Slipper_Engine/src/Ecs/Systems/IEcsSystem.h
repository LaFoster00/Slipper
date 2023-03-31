#pragma once

namespace Slipper
{
template<typename SystemT> struct IEcsSystem
{
	virtual ~IEcsSystem() = default;
	virtual void Execute(entt::registry &Registry) = 0;

    static SystemT &Get()
    {
        static SystemT system;
        return system;
    }

    static inline bool systemAdded = EcsInterface::AddSystem<SystemT>();
};
}  // namespace Slipper