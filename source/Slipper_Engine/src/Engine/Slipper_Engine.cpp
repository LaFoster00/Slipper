#include "Slipper_Engine.h"

#include "Core/Application.h"
#include "GraphicsEngine.h"
#include "Window.h"


namespace Slipper
{
SlipperEngine::SlipperEngine(ApplicationInfo &ApplicationInfo) : Application(ApplicationInfo)
{
    GraphicsEngine::Get().SetupDebugRender(window->GetSurface());
}

void SlipperEngine::Run()
{
    Application::Run();
}
}  // namespace Slipper