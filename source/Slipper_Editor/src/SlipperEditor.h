#pragma once
#include "Core/Application.h"

namespace Slipper::Editor
{
class Editor;

class SlipperEditor : public Application
{
 public:
    using Application::Application;

    void Init() override;

private:
    void UpdateEditor(NonOwningPtr<RenderingStage> RenderingStage) const;

 private:
    NonOwningPtr<Editor> m_editor;
    NonOwningPtr<Gui> m_editorGui;
};
}  // namespace Slipper::Editor
