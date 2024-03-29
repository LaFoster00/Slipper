#include "SlipperEditor.h"

#include "ComputeShaderTest.h"
#include "Core/AppComponents/Ecs.h"
#include "Core/AppComponents/Gui.h"
#include "Editor.h"
#include "GraphicsEngine.h"
#include "Window.h"

int main(int argc, char *argv[])
{
    try
    {
        Slipper::ApplicationInfo app_info{"Slipper Engine "};
        const OwningPtr<Slipper::Editor::SlipperEditor> app = new Slipper::Editor::SlipperEditor();
        app->Init(app_info);
        app->Run();
        app->Shutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

namespace Slipper::Editor
{
    void SlipperEditor::Init(ApplicationInfo &ApplicationInfo)
    {
        Application::Init(ApplicationInfo);
        GPU::GraphicsEngine::Get().SetupDebugRender(window->GetContext());
        m_editor = AddComponentBefore(new Editor(), ecsComponent);
        m_editorGui = AddComponent(new Gui("Editor Gui", GPU::GraphicsEngine::Get().windowRenderPass, true));

        AddAdditionalRenderStageUpdate(GPU::GraphicsEngine::Get().windowRenderingStage,
                                       [this](const NonOwningPtr<GPU::RenderingStage> Stage) { UpdateEditor(Stage); });

        m_computeShaderTest = AddComponent(new ComputeShaderTest());
    }

    void SlipperEditor::UpdateEditor(NonOwningPtr<GPU::RenderingStage> RenderingStage) const
    {
        m_editorGui->StartNewFrame();

        for (auto &app_component : appComponents)
        {
            if (const auto editor_app_component = dynamic_cast<EditorAppComponent *>(app_component.get()))
            {
                editor_app_component->OnEditorGuiUpdate();
            }
        }

        m_editorGui->EndNewFrame(RenderingStage);
    }
}  // namespace Slipper::Editor
