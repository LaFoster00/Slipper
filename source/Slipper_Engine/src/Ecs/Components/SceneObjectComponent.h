#pragma once

namespace Slipper
{
struct SceneObjectComponent : Slipper::IEcsComponent<SceneObjectComponent>
{
    SceneObjectComponent(const std::string_view Name = "SceneObject")
    {
        if (Name == "SceneObject") {
            SetName(std::format("SceneObject_{}", numObjects));
            numObjects++;
        }
        else {
            SetName(Name);
        }
    }

    void SetName(std::string_view Name)
    {
        Name.copy(name, std::min(Name.size(), sizeof(name) - 1));
        name[Name.size()] = '\0';
    }

    std::string GetName() const
    {
        return std::string(name);
    }

    char name[MAX_SCENE_OBJECT_NAME_LENGTH];
    static inline int numObjects = 0;
};
}  // namespace Slipper