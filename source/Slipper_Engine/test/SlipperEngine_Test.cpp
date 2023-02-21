#include "SlipperEngine_Test.h"

#include "GraphicsEngine.h"
#include "TestGui.h"
#include "Window.h"

int main(int argc, char *argv[])
{
    try {
        Slipper::ApplicationInfo app_info{"Slipper Engine "};
        auto app = new SlipperEngineTest(app_info);
        TestGui *TestGui = new ::TestGui();
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

SlipperEngineTest::SlipperEngineTest(Slipper::ApplicationInfo &ApplicationInfo)
    : Application(ApplicationInfo)
{
    Slipper::GraphicsEngine::Get().SetupDebugRender(window->GetSurface());
}
