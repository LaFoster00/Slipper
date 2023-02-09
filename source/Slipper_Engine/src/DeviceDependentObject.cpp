#include "DeviceDependentObject.h"

#include "Setup/Device.h"

namespace Slipper
{
DeviceDependentObject::DeviceDependentObject() : device(Device::Get())
{
}
}  // namespace Slipper