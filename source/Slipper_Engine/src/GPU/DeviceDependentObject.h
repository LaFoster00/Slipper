#pragma once

namespace Slipper
{
class VKDevice;

class DeviceDependentObject
{
 public:
    DeviceDependentObject();

 public:
    VKDevice &device;
};
}  // namespace Slipper
