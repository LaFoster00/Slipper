#pragma once

#include "Engine.h"

class Device;

class DeviceDependentObject
{
public:
    DeviceDependentObject();

public:
    Device &device;
};
