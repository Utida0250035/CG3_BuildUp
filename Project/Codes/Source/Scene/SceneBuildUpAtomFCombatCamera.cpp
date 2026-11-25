#include "Scene/SceneBuildUpAtomFCombatCamera.h"

#include "Camera/Camera.h"
#include "Camera/StageViewCamera.h"
#include "Camera/StageViewFramingBounds.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneBuildUpStageCameraFraming.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

#include <memory>
#include <vector>

namespace Atrum::SceneBuildUpAtomFCombatCamera {

namespace {

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

StageViewCameraPose ComputeAtomFFollowPose(
    const Math::Vector3 &pivot,
    const AtomF::CombatCameraFollowEyeOffset &eyeOffset) {
  StageViewCameraPose pose{};
  pose.pivot = pivot;
  pose.eye = {pivot.x + eyeOffset.offsetXMeters,
              pivot.y + eyeOffset.offsetYMeters,
              pivot.z + eyeOffset.offsetZMeters};
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

  const AtomF::CombatCameraParameters cameraParams =
      AtomF::ResolveCombatCameraParameters();
  ApplyStageViewCameraPose(
      camera, ComputeAtomFFollowPose(pivot, cameraParams.followEyeOffset));
  PublishStageViewFramingBounds(scene.GetStageViewFramingBounds());
  scene.PrepareCamerasForDraw();
}

StageViewFramingBounds BuildFramingBoundsFromParameters(
    const AtomF::CombatCameraFraming &framing) {
  StageViewFramingBounds bounds{};
  bounds.leftEndX = framing.leftEndXMeters;
  bounds.rightEndX = framing.rightEndXMeters;
  bounds.groundHeight = framing.groundHeightMeters;
  bounds.barrageSpawnTopY = framing.barrageSpawnTopYMeters;
  bounds.pivotZ = framing.pivotZMeters;
  return bounds;
}

} // namespace

bool ApplyOnEnterIfAtomFScene(SceneBuildUp &scene) {
  if (!AtomF::PathEndsWithAtomFJson(scene.SceneJsonPath())) {
    return false;
  }

  const AtomF::CombatCameraParameters cameraParams =
      AtomF::ResolveCombatCameraParameters();
  scene.SetStageViewFramingBounds(
      BuildFramingBoundsFromParameters(cameraParams.framing));
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