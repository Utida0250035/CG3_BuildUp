#include "System/Editor/Scene/EditorSceneRuntimeEditorAssetPathPreviewLink.h"

#include "System/Editor/Scene/EditorSceneRuntimeBuildWarnings.h"
#include "System/Editor/Scene/EditorSceneRuntimeDrawPreviewApply.h"
#include "System/Entity/Components/CmpEditorAssetPath.h"
#include "System/Entity/Components/CmpEditorTextureBillboard.h"
#include "System/Entity/Components/CmpMesh.h"
#include "System/Entity/Entity.h"

#include <filesystem>
#include <sstream>

namespace Atrum::EntityEditor::EditorSceneRuntimeEditorAssetPathPreviewLink {

namespace {

void RecordNeedsDrawSlot(EditorSceneRuntimeBuildWarnings *warnings,
                         const std::string &entityLabel,
                         const std::string &drawType,
                         const std::string &reason) {
  if (warnings == nullptr) {
    return;
  }
  std::ostringstream oss;
  oss << "プレビュー: [" << entityLabel << "] CmpEditorAssetPath — "
      << drawType << " の Draw スロットが無いため " << reason
      << "（Data から Draw を自動追加しません）";
  warnings->lines.push_back(oss.str());
}

std::string ExtensionLower(const std::string &path) {
  namespace fs = std::filesystem;
  std::string ext = fs::path(path).extension().string();
  for (char &ch : ext) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return ext;
}

} // namespace

void Apply(Entity &entity, const std::string &entityLabel,
           EditorSceneRuntimeBuildWarnings *warnings) {
  if (!entity.HasDataCmp<CmpEditorAssetPath>()) {
    return;
  }
  const CmpEditorAssetPath *data = entity.GetDataCmp<CmpEditorAssetPath>();
  const std::string path = data->AssetPath();
  if (path.empty()) {
    return;
  }
  namespace fs = std::filesystem;
  if (!fs::exists(fs::path(path))) {
    return;
  }

  const std::string extLower = ExtensionLower(path);

  if (data->LoadObjAsMesh() && extLower == ".obj") {
    if (entity.HasDrawCmp<CmpMesh>()) {
      CmpMesh *mesh = entity.GetDrawCmp<CmpMesh>();
      EditorSceneRuntimeDrawPreviewApply::TryApplyCmpMeshObjAssetPath(*mesh,
                                                                     path);
      if (mesh != nullptr) {
        mesh->SetLightingEnabled(true);
      }
    } else if (data->LoadObjAsMesh()) {
      RecordNeedsDrawSlot(warnings, entityLabel, "CmpMesh", "obj を表示できません");
    }
  }

  if (data->LoadImageAsBillboard() &&
      EditorSceneRuntimeDrawPreviewApply::IsImageAssetExtensionLower(
          extLower)) {
    if (entity.HasDrawCmp<CmpEditorTextureBillboard>()) {
      CmpEditorTextureBillboard *billboard =
          entity.GetDrawCmp<CmpEditorTextureBillboard>();
      EditorSceneRuntimeDrawPreviewApply::TryApplyCmpEditorTextureBillboardPath(
          *billboard, path);
    } else {
      RecordNeedsDrawSlot(warnings, entityLabel, "CmpEditorTextureBillboard",
                          "画像を表示できません");
    }
  }
}

} // namespace Atrum::EntityEditor::EditorSceneRuntimeEditorAssetPathPreviewLink