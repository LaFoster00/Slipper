#include "TestGui.h"

#include <imgui.h>

void TestGui::OnGuiRender()
{
    AppComponent::OnGuiRender();

    bool open = true;
    ImGui::ShowMetricsWindow(&open);
}
