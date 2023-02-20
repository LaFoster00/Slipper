#pragma once

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
