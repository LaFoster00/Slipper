#pragma once

namespace Slipper
{
    class Object<>;
    class ObjectDatabase
    {
        friend Object<>;

     public:
        static const std::span<Ref<const type_info>> GetObjectTypes()
        {
            return objectInfos;
        }

     private:
        static void AddObjectInfo(const type_info &info)
        {
            objectInfos.emplace_back(info);
        }

     private:
        static inline std::vector<Ref<const type_info>> objectInfos;
    };

    template<typename T>
    class Object
    {
     public:
        virtual ~Object() = default;
        static const type_info &GetTypeInfo()
        {
            return m_typeInfo;
        }

    private:
        static const type_info &GenerateTypeInfo()
        {
            const auto &type = typeid(T);
            ObjectDatabase::AddObjectInfo(type);
            return typeid(T);
        }

     private:
        static inline const type_info &m_typeInfo = GenerateTypeInfo();
    };
}  // namespace Slipper
