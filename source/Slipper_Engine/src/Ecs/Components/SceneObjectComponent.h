#pragma once

namespace Slipper
{
struct SceneObjectComponent : Slipper::IEcsComponent<SceneObjectComponent>
{
    SceneObjectComponent(const std::string_view Name = "SceneObject")
    {
        if (Name == "SceneObject") {
            name = std::format("SceneObject_{}", numObjects);
            numObjects++;
        }
        else {
            name = Name;
        }
    }
    std::string name;
    static inline int numObjects = 0;
};
}  // namespace Slipper