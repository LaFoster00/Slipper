#pragma once

namespace Slipper
{
template<typename T> class Object
{
 public:
    virtual ~Object() = default;
    static const type_info &GetTypeInfo()
    {
        return m_typeInfo;
    }

 private:
    constexpr static type_info &m_typeInfo = typeid(T);
};
}  // namespace Slipper