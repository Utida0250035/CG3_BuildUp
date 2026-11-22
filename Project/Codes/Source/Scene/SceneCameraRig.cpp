#include "Scene/SceneCameraRig.h"

#include "Scene/Scene.h"
#include "Scene/SceneDebugCameraInputPolicy.h"
#include "Scene/ScenePrimaryCameraResolve.h"
#include "System/Entity/Components/CmpCamera.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"

namespace Atrum {

SceneCameraRig::SceneCameraRig() = default;

SceneCameraRig::~SceneCameraRig() = default;

SceneCameraRig::SceneCameraRig(SceneCameraRig &&) noexcept = default;

SceneCameraRig &SceneCameraRig::operator=(SceneCameraRig &&) noexcept = default;

const std::vector<std::unique_ptr<Entity>> *
SupplementalEntitiesFromScene(const Scene *ownerScene) {
  if (ownerScene == nullptr) {
    return nullptr;
  }
  return ownerScene->GetSupplementalCameraEntities();
}

void SceneCameraRig::EnsureAutoSpawnedCameras(const Scene *ownerScene) {
  if (autoSpawnAttempted_ && FindPrimaryCmpCamera(ownerScene) != nullptr) {
    return;
  }
  autoSpawnAttempted_ = true;

  if (FindPrimaryCmpCamera(ownerScene) != nullptr) {
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

CmpCamera *SceneCameraRig::FindPrimaryCmpCamera(const Scene *ownerScene) {
  return ResolvePrimaryCmpCamera(entities_,
                                 SupplementalEntitiesFromScene(ownerScene));
}

const CmpCamera *SceneCameraRig::FindPrimaryCmpCamera(
    const Scene *ownerScene) const {
  return ResolvePrimaryCmpCamera(entities_,
                                 SupplementalEntitiesFromScene(ownerScene));
}

CmpCamera *SceneCameraRig::GetPrimaryCmpCamera(const Scene *ownerScene) {
  EnsureAutoSpawnedCameras(ownerScene);
  return FindPrimaryCmpCamera(ownerScene);
}

const CmpCamera *SceneCameraRig::GetPrimaryCmpCamera(
    const Scene *ownerScene) const {
  return FindPrimaryCmpCamera(ownerScene);
}

Camera *SceneCameraRig::GetPrimaryCamera(const Scene *ownerScene) {
  CmpCamera *cmp = GetPrimaryCmpCamera(ownerScene);
  return cmp ? cmp->GetCamera() : nullptr;
}

const Camera *SceneCameraRig::GetPrimaryCamera(const Scene *ownerScene) const {
  const CmpCamera *cmp = FindPrimaryCmpCamera(ownerScene);
  return cmp ? cmp->GetCamera() : nullptr;
}

void SceneCameraRig::PrepareActiveCameraBeforeDraw(const Scene &scene) {
  EnsureAutoSpawnedCameras(&scene);
  if (CmpCamera *cmp = FindPrimaryCmpCamera(&scene)) {
    cmp->PrepareBeforeDraw(AllowsPlayerDebugCameraInput(scene));
  }
}

void SceneCameraRig::TickSpawnedEntities() {
  for (const std::unique_ptr<Entity> &entity : entities_) {
    if (entity && entity->IsActive()) {
      entity->Update();
    }
  }
}

} // namespace Atrum