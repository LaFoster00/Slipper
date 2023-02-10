#pragma once

class SlipperObject
{
public:
    virtual void OnCreate() {};
    virtual void OnUpdate() {};
    virtual void OnDestroy() {};

private:
   SlipperObject() = default;
   virtual ~SlipperObject() = default;
};