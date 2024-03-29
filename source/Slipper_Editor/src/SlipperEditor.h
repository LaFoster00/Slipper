#pragma once
#include "Core/Application.h"

namespace Slipper
{
    class Gui;
}

namespace Slipper::Editor
{
	class ComputeShaderTest;
	class Editor;

class SlipperEditor : public Application
{
 public:
    using Application::Application;

    void Init(ApplicationInfo &ApplicationInfo) override;

    NonOwningPtr<Editor> GetEditor() const
    {
        return m_editor;
    }

    NonOwningPtr<Gui> GetEditorGui() const
    {
        return m_editorGui;
    }

 private:
    void UpdateEditor(NonOwningPtr<GPU::RenderingStage> RenderingStage) const;

 private:
    NonOwningPtr<Editor> m_editor;
    NonOwningPtr<Gui> m_editorGui;
    NonOwningPtr<ComputeShaderTest> m_computeShaderTest;
};
}  // namespace Slipper::Editor
