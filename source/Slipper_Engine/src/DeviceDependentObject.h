#pragma once

#include "Engine.h"

namespace Slipper
{
class Device;

class DeviceDependentObject
{
 public:
    DeviceDependentObject();

 public:
    Device &device;
};
}  // namespace Slipper
