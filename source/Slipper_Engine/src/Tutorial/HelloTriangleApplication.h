#pragma once

#include "Rendering/GraphicsPipeline/GraphicsPipeline.h"
#include "Rendering/Presentation/Surface.h"
#include "Rendering/Setup/Device.h"
#include "Rendering/Setup/Instance.h"
#include "Window/Window.h"

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

constexpr bool PRINT_CREATION_DETAILS = true;

class GraphicsEngine;

class HelloTriangleApplication
{
 public:
    void Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        Cleanup();
    }

 private:
#pragma region MainFunctions

    void InitWindow();

    void InitVulkan();

    void MainLoop();

    void Cleanup();

#pragma endregion MainFunctions

 private:
    Window *window = nullptr;
    Instance *instance = nullptr;
    Surface *surface = nullptr;
    Device *device = nullptr;
    GraphicsEngine *graphics = nullptr;
};