#include "System/Editor/Scene/EditorSceneRuntimeDrawPreviewApply.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/AssetModel.h"
#include "System/Entity/Components/CmpEditorTextureBillboard.h"
#include "System/Entity/Components/CmpMesh.h"

#include <cctype>
#include <filesystem>
#include <memory>

namespace Atrum::EntityEditor::EditorSceneRuntimeDrawPreviewApply {

namespace {

std::string ToLowerAscii(std::string value) {
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

} // namespace

bool IsImageAssetExtensionLower(const std::string &extensionLower) {
  return extensionLower == ".png" || extensionLower == ".jpg" ||
         extensionLower == ".jpeg";
}

void TryApplyCmpMeshObjAssetPath(CmpMesh &mesh, const std::string &path) {
  if (path.empty()) {
    return;
  }
  namespace fs = std::filesystem;
  const fs::path assetPath(path);
  if (!fs::exists(assetPath)) {
    return;
  }
  const std::string extLower = ToLowerAscii(assetPath.extension().string());
  if (extLower != ".obj") {
    return;
  }
  AtrumEngine *engine = AtrumEngine::Instance();
  if (engine == nullptr) {
    return;
  }
  const std::string objDirectory = assetPath.parent_path().generic_string();
  const std::string objFileName = assetPath.filename().string();
  const std::string mtlFileName = assetPath.stem().string() + ".mtl";
  const std::shared_ptr<AssetModel> model =
      engine->GetModel(objDirectory, objFileName, objDirectory, mtlFileName);
  if (model) {
    mesh.SetModel(model.get());
  }
}

void TryApplyCmpEditorTextureBillboardPath(CmpEditorTextureBillboard &billboard,
                                           const std::string &path) {
  if (path.empty()) {
    return;
  }
  namespace fs = std::filesystem;
  if (!fs::exists(fs::path(path))) {
    return;
  }
  AtrumEngine *engine = AtrumEngine::Instance();
  if (engine == nullptr) {
    return;
  }
  const std::string extLower =
      ToLowerAscii(fs::path(path).extension().string());
  if (!IsImageAssetExtensionLower(extLower)) {
    return;
  }
  const uint32_t textureIndex = engine->GetTexture(path);
  if (textureIndex != 0) {
    billboard.SetTextureIndex(textureIndex);
  }
}

} // namespace Atrum::EntityEditor::EditorSceneRuntimeDrawPreviewApply