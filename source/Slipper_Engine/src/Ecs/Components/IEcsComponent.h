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
}  // namespace Slipper