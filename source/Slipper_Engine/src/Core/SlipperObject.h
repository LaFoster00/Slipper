#pragma once

class SlipperObject
{
public:
    virtual void OnInit() {};
    virtual void OnUpdate() {};
    virtual void OnDestroy() {};

protected:
    SlipperObject() = default;
    virtual ~SlipperObject() = default;
};