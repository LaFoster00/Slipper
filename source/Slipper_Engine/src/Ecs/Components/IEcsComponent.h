#pragma once

namespace Slipper
{
struct IEcsComponentBase
{
};

template<typename ComponentT> struct IEcsComponent : IEcsComponentBase
{
    static inline const bool TypeInfoGenerated = EcsInterface::InitComponentLookup<ComponentT>();
};

template<typename ComponentT> struct IEcsComponentWithEntity : IEcsComponent<ComponentT>
{
    IEcsComponentWithEntity(Entity Entity) : entity(Entity)
    {
    }

    Entity entity;
};
}  // namespace Slipper