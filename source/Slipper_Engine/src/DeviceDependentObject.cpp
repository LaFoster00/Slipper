#include "DeviceDependentObject.h"

namespace Slipper
{
DeviceDependentObject::DeviceDependentObject() : device(Device::Get())
{
}
}  // namespace Slipper