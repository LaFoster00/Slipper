#include "SlipperEditor.h"

#include "Editor.h"
#include "GraphicsEngine.h"
#include "Window.h"

int main(int argc, char *argv[])
{
    try {
        Slipper::ApplicationInfo app_info{"Slipper Engine "};
        auto app = new Slipper::Editor::SlipperEditor(app_info);
        app->Init();
        Slipper::Editor::Editor *TestGui = new Slipper::Editor::Editor();
        app->AddComponent(TestGui);
        app->Run();
        delete app;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

namespace Slipper::Editor
{
void SlipperEditor::Init()
{
    Application::Init();
    GraphicsEngine::Get().SetupDebugRender(window->GetSurface());
}

}  // namespace Slipper::Editor