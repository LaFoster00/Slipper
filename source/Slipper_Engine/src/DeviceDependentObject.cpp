#include "DeviceDependentObject.h"

#include "Setup/Device.h"

DeviceDependentObject::DeviceDependentObject() : device(Device::Get())
{

}
