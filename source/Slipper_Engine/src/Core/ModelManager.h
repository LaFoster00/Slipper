#pragma once

namespace Slipper
{
class Model;

class ModelManager
{
 public:
    static NonOwningPtr<Model> Load(std::string_view Filepath);
    static NonOwningPtr<Model> GetModel(std::string_view Name);
    static void Shutdown();

 private:
    static std::vector<OwningPtr<Model>> m_models;
    static std::map<size_t, NonOwningPtr<Model>> m_namedModels;
};
}  // namespace Slipper