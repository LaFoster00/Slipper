#pragma once

class Device;

class DeviceDependentObject
{
public:
    DeviceDependentObject();

public:
    Device &device;
};
