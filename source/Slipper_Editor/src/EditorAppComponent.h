#pragma once
#include "Core/AppComponent.h"

namespace Slipper::Editor
{
    class EditorAppComponent : public AppComponent
    {
     public:
        using AppComponent::AppComponent;
        virtual void OnEditorGuiUpdate(){};
    };
}  // namespace Slipper::Editor
