#pragma once

#include <optional>
#include <functional>
#include <any>
#include <memory>
#include <algorithm>
#include <ranges>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <fstream>

#include <signal.h>

#include <unordered_map>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>


#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <ImGuizmo.h>

#include "stb_image.h"


#include "common_defines.h"
#include "Engine.h"
#include "Util/StringUtil.h"
#include "Setup/GraphicsSettings.h"
#include "Setup/VulkanInstance.h"
#include "Setup/Device.h"
#include "DeviceDependentObject.h"