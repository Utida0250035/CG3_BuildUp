#include "System/Editor/Scene/EditorSceneRuntimeMainCameraSupplement.h"

#include "Scene/ScenePrimaryCameraResolve.h"
#include "System/Editor/Scene/EditorSceneRuntimeBuildWarnings.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"

namespace Atrum::EntityEditor::EditorSceneRuntimeMainCameraSupplement {

namespace {

const std::vector<std::unique_ptr<Entity>> &EmptyRigEntities() {
  static const std::vector<std::unique_ptr<Entity>> kEmpty{};
  return kEmpty;
}

bool HasPrimaryCameraInList(
    const std::vector<std::unique_ptr<Entity>> &entities) {
  return ResolvePrimaryCmpCamera(EmptyRigEntities(), &entities) != nullptr;
}

} // namespace

void EnsureInEntityList(std::vector<std::unique_ptr<Entity>> &entities,
                        EditorSceneRuntimeBuildWarnings *warnings) {
  if (HasPrimaryCameraInList(entities)) {
    return;
  }

  std::unique_ptr<Entity> spawned =
      EntityFactory::TryMainCameraFromDefaultPrefab();
  if (!spawned) {
    spawned = EntityFactory::MainCameraEntity();
  }
  if (!spawned) {
    if (warnings != nullptr) {
      warnings->lines.push_back(
          "プレビュー: camera.main 相当の CmpCamera を自動生成できませんでした");
    }
    return;
  }

  if (warnings != nullptr) {
    warnings->lines.push_back(
        "プレビュー: シーンに primary カメラが無いため MainCamera を自動追加しました "
        "（SceneCameraRig と同契約）");
  }
  entities.push_back(std::move(spawned));
}

} // namespace Atrum::EntityEditor::EditorSceneRuntimeMainCameraSupplement