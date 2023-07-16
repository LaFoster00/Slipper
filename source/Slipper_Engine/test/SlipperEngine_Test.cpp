#include "SlipperEngine_Test.h"

#include "GraphicsEngine.h"
#include "TestGui.h"
#include "Window.h"

int main(int argc, char *argv[])
{
    try {
        Slipper::ApplicationInfo app_info{"Slipper Engine "};
        const auto app = new SlipperEngineTest();
        app->Init(app_info);
        const auto test_gui = new ::TestGui();
        app->AddComponent(test_gui);
        app->Run();
        delete app;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void SlipperEngineTest::Init(Slipper::ApplicationInfo &ApplicationInfo)
{
    Application::Init(ApplicationInfo);
    Slipper::GraphicsEngine::Get().SetupDebugRender(window->GetSurface());
}
