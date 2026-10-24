#include "Camera/StageViewCamera.h"

#include "Camera/Camera.h"
#include "Camera/CameraFlipHorizontal.h"
#include "Camera/CameraFlipVertical.h"
#include "Engine/Platform/Window/Window.h"
#include "Math/Lerp.h"
#include "System/StageBounds.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

namespace {

constexpr float kPerspectiveFovY = 0.5f;

float GetAspectRatio() {
  return static_cast<float>(Window::kDefaultClientWidth) /
         static_cast<float>(Window::kDefaultClientHeight);
}

StageViewCameraPose ComputeViewPose(const Math::Vector3 &pivot, float viewHalfHeight,
                                    float viewHalfWidth, float distanceScale) {
  const float aspectRatio = GetAspectRatio();
  const float halfTan = std::tan(kPerspectiveFovY * 0.5f);
  const float distanceForHeight = viewHalfHeight / halfTan;
  const float distanceForWidth =
      viewHalfWidth / (halfTan * aspectRatio);
  const float eyeBackFromStage =
      std::max(distanceForHeight, distanceForWidth) * distanceScale;

  StageViewCameraPose pose{};
  pose.pivot = pivot;
  pose.eye = {pivot.x, pivot.y, pivot.z - eyeBackFromStage};
  return pose;
}

void SetupPerspectiveMatrix(Camera *camera) {
  camera->SetPerspectiveFovMatrix(Math::Matrix4x4::PerspectiveFov(
      kPerspectiveFovY, GetAspectRatio(), 0.125f, 128.0f));
}

} // namespace

StageViewCameraPose ComputeStageBattleViewPose() {
  const float centerX = (kLeftEndX + kRightEndX) * 0.5f;
  const float bottomY = kGroundHeight;
  const float topY = kBarrageSpawnTopY;
  const float centerY = (bottomY + topY) * 0.5f;
  const float viewHeight = (topY - bottomY) * 0.5f;
  const float viewWidth = (kRightEndX - kLeftEndX) * 0.5f;

  const Math::Vector3 pivot{centerX, centerY, 0.0f};
  return ComputeViewPose(pivot, viewHeight, viewWidth, 1.08f);
}

StageViewCameraPose ComputeFocusViewPose(const Math::Vector3 &focusWorld,
                                         float subjectHalfHeight,
                                         float subjectHalfWidth) {
  return ComputeViewPose(focusWorld, subjectHalfHeight, subjectHalfWidth,
                         1.12f);
}

void ApplyStageViewCameraPose(Camera *camera, const StageViewCameraPose &pose) {
  if (camera == nullptr) {
    return;
  }

  SetupPerspectiveMatrix(camera);
  camera->LookAt(pose.eye, pose.pivot, Math::Vector3::UpLh());
  FlipCameraHorizontal(*camera);
  FlipCameraVertical(*camera);
  camera->UpdateMatrix();
}

StageViewCameraPose LerpStageViewCameraPose(const StageViewCameraPose &from,
                                            const StageViewCameraPose &to,
                                            const float t) {
  StageViewCameraPose result{};
  result.eye = Math::Lerp(from.eye, to.eye, t);
  result.pivot = Math::Lerp(from.pivot, to.pivot, t);
  return result;
}

} // namespace Atrum
