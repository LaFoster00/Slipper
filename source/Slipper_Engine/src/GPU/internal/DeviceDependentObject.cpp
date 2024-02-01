#include "DeviceDependentObject.h"

namespace Slipper
{
DeviceDependentObject::DeviceDependentObject() : device(VKDevice::Get())
{
}
}  // namespace Slipper