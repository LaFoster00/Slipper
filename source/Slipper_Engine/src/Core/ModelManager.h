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
};
}  // namespace Slipper