#include "ModelManager.h"

#include "File.h"
#include "Model/Model.h"

namespace Slipper
{
std::vector<OwningPtr<Model>> ModelManager::m_models;
std::map<size_t, NonOwningPtr<Model>> ModelManager::m_namedModels;

NonOwningPtr<Model> ModelManager::Load(std::string_view Filepath)
{
    const auto model_name = File::remove_file_type_from_name(File::get_file_name_from_path(Filepath));
    const auto model_name_hash = StringViewHash{}(model_name);

    if (m_namedModels.contains(model_name_hash)) {
        LOG_FORMAT("Returned already loaded model '{}'", model_name)
        return m_namedModels.at(model_name_hash);
    }

    LOG_FORMAT("Loaded model '{}' in filepath '{}'", model_name, Filepath);
    auto &new_model = m_models.emplace_back(new Model(Filepath));
    return m_namedModels.emplace(model_name_hash, new_model).first->second;
}

NonOwningPtr<Model> ModelManager::GetModel(std::string_view Name)
{
    const auto model_name_hash = StringViewHash{}(Name);
    if (m_namedModels.contains(model_name_hash)) {
        return m_namedModels.at(model_name_hash);
    }
    LOG_FORMAT("Model '{}' does not exist!", Name);
    return nullptr;
}

void ModelManager::Shutdown()
{
    m_namedModels.clear();
    m_models.clear();
}
}  // namespace Slipper
