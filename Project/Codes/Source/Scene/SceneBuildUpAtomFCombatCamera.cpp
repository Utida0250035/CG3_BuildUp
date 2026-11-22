#include "Scene/SceneBuildUpAtomFCombatCamera.h"

#include "Camera/Camera.h"
#include "Camera/StageViewCamera.h"
#include "Camera/StageViewFramingBounds.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneBuildUpStageCameraFraming.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

#include <memory>
#include <vector>

namespace Atrum::SceneBuildUpAtomFCombatCamera {

namespace {

/// 斜め上から見下ろすオフセット（ローカル LH: Y 上、Z を手前に）
constexpr float kEyeHeightAbovePivotY = 11.0f;
constexpr float kEyeSideOffsetX = -5.0f;
constexpr float kEyeOffsetZ = -7.0f;

const Entity *FindPlayerEntity(
    const std::vector<std::unique_ptr<Entity>> &entities) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  for (const std::unique_ptr<Entity> &entity : entities) {
    if (entity == nullptr || !entity->IsActive()) {
      continue;
    }
    if (tags.EntityHasRole(*entity, "player")) {
      return entity.get();
    }
  }
  return nullptr;
}

Math::Vector3 ResolveCameraPivot(
    const std::vector<std::unique_ptr<Entity>> &entities) {
  const Entity *player = FindPlayerEntity(entities);
  if (player != nullptr) {
    return player->GetWorldPosition();
  }
  return {0.0f, 0.5f, 0.0f};
}

StageViewCameraPose ComputeAtomFFollowPose(const Math::Vector3 &pivot) {
  StageViewCameraPose pose{};
  pose.pivot = pivot;
  pose.eye = {pivot.x + kEyeSideOffsetX, pivot.y + kEyeHeightAbovePivotY,
              pivot.z + kEyeOffsetZ};
  return pose;
}

void ApplyFollowPose(SceneBuildUp &scene) {
  Camera *camera = scene.GetCamera();
  if (camera == nullptr) {
    return;
  }

  const std::vector<std::unique_ptr<Entity>> *entities =
      scene.GetSupplementalCameraEntities();
  const Math::Vector3 pivot =
      entities != nullptr ? ResolveCameraPivot(*entities)
                          : Math::Vector3{0.0f, 0.5f, 0.0f};

  ApplyStageViewCameraPose(camera, ComputeAtomFFollowPose(pivot));
  PublishStageViewFramingBounds(scene.GetStageViewFramingBounds());
  scene.PrepareCamerasForDraw();
}

} // namespace

bool ApplyOnEnterIfAtomFScene(SceneBuildUp &scene) {
  if (!AtomF::PathEndsWithAtomFJson(scene.SceneJsonPath())) {
    return false;
  }

  StageViewFramingBounds bounds{};
  bounds.leftEndX = -16.0f;
  bounds.rightEndX = 16.0f;
  bounds.groundHeight = 0.0f;
  bounds.barrageSpawnTopY = 4.0f;
  bounds.pivotZ = 0.0f;
  scene.SetStageViewFramingBounds(bounds);
  ApplyFollowPose(scene);
  return true;
}

void MaintainSceneCamera(SceneBuildUp &scene) {
  if (AtomF::PathEndsWithAtomFJson(scene.SceneJsonPath())) {
    ApplyFollowPose(scene);
    return;
  }

  SceneBuildUpStageCamera::MaintainLockedStageCamera(scene);
}

} // namespace Atrum::SceneBuildUpAtomFCombatCamera