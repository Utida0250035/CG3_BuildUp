#include "Scene/SceneCameraRig.h"

#include "Scene/Scene.h"
#include "System/Entity/Components/CmpCamera.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/Hash64.h"

namespace Atrum {

namespace {

constexpr const char *kMainCameraTag = "camera.main";

bool EntityHasMainCameraTag(const Entity &entity) {
  return entity.HasTagHash(Hash64(kMainCameraTag));
}

} // namespace

void SceneCameraRig::EnsureAutoSpawnedCameras() {
  if (autoSpawnAttempted_ && FindPrimaryCmpCamera() != nullptr) {
    return;
  }
  autoSpawnAttempted_ = true;

  if (FindPrimaryCmpCamera() != nullptr) {
    return;
  }

  std::unique_ptr<Entity> spawned =
      EntityFactory::TryMainCameraFromDefaultPrefab();
  if (!spawned) {
    spawned = EntityFactory::MainCameraEntity();
  }
  if (spawned) {
    entities_.push_back(std::move(spawned));
  }
}

void SceneCameraRig::Clear() {
  for (auto &entity : entities_) {
    if (entity) {
      entity->Finalize();
    }
  }
  entities_.clear();
  autoSpawnAttempted_ = false;
}

CmpCamera *SceneCameraRig::FindPrimaryCmpCamera() const {
  const Entity *primaryEntity = nullptr;
  for (const auto &entity : entities_) {
    if (!entity || !entity->HasUpdCmp<CmpCamera>()) {
      continue;
    }
    if (EntityHasMainCameraTag(*entity)) {
      primaryEntity = entity.get();
      break;
    }
    if (primaryEntity == nullptr) {
      primaryEntity = entity.get();
    }
  }
  if (primaryEntity == nullptr) {
    return nullptr;
  }
  return primaryEntity->GetUpdCmp<CmpCamera>();
}

CmpCamera *SceneCameraRig::GetPrimaryCmpCamera() {
  EnsureAutoSpawnedCameras();
  return FindPrimaryCmpCamera();
}

const CmpCamera *SceneCameraRig::GetPrimaryCmpCamera() const {
  return FindPrimaryCmpCamera();
}

Camera *SceneCameraRig::GetPrimaryCamera() {
  CmpCamera *cmp = GetPrimaryCmpCamera();
  return cmp ? cmp->GetCamera() : nullptr;
}

const Camera *SceneCameraRig::GetPrimaryCamera() const {
  const CmpCamera *cmp = FindPrimaryCmpCamera();
  return cmp ? cmp->GetCamera() : nullptr;
}

void SceneCameraRig::PrepareActiveCameraBeforeDraw(const Scene &scene) {
  EnsureAutoSpawnedCameras();
  if (CmpCamera *cmp = FindPrimaryCmpCamera()) {
    cmp->PrepareBeforeDraw(scene.IsStageViewCameraLocked(),
                           scene.AllowDebugCameraControl());
  }
}

} // namespace Atrum
