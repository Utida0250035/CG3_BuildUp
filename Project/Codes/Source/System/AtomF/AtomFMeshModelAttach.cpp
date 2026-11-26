#include "System/AtomF/AtomFMeshModelAttach.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/ModelStorage.h"
#include "ForDebug/Log.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/GameAssetsPaths.h"

#include <filesystem>

namespace Atrum::AtomF {

std::shared_ptr<AssetModel> LoadMeshModel(const std::string &modelName) {
  if (modelName.empty()) {
    return nullptr;
  }

  const std::string directory =
      std::string(GameAssets::kObjectsDirectory) + "/" + modelName;
  const std::string objFileName = modelName + ".obj";
  const std::string mtlFileName = modelName + ".mtl";

  // ModelStorage はファイルが無いと assert で止まるため、先に確かめて球で代替する。
  if (!std::filesystem::exists(directory + "/" + objFileName) ||
      !std::filesystem::exists(directory + "/" + mtlFileName)) {
    Debug::LogFile::Instance()->Log("atomF model not found: " + directory +
                                    "/" + objFileName);
    return nullptr;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  std::shared_ptr<AssetModel> model =
      engine->GetModel(directory, objFileName, directory, mtlFileName);
  engine->GetModelStorage()->RetainModel(model);
  return model;
}

void AttachMeshModel(CmpMeshSphere &mesh, const std::string &modelName) {
  if (std::shared_ptr<AssetModel> model = LoadMeshModel(modelName)) {
    mesh.SetModel(std::move(model));
  }
}

} // namespace Atrum::AtomF
