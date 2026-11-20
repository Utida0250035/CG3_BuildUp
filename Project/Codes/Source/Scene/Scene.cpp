#include "Scene/Scene.h"

#include "Camera/StageViewCamera.h"
#include "Math/Matrix4x4.h"
#if defined(DEVELOPMENT)
#include "System/Editor/Camera/StageViewCameraTuning.h"
#endif
#include "System/Entity/Entity.h"
#include "Engine/AtrumEngine.h"
#include "Scene/CommandChangeScene.h"

#include <cmath>

#if defined(DEVELOPMENT)
#include "ForDebug/SubsystemFrameProfiler.h"
#endif
#if defined(DEVELOPMENT) && !defined(NDEBUG)
#include "ForDebug/DebugCamera.h"
#endif

namespace Atrum {

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::EnsureParticleEmitterManager() {
  if (!particleEmitterManager_) {
    particleEmitterManager_ =
        std::make_unique<Particle::Emitter::EmitterManager>();
  }
}

void Scene::ReleaseParticleEmitterManager() {
  particleEmitterManager_.reset();
}

void Scene::EnsureCommonResources() { EnsureParticleEmitterManager(); }

void Scene::ReleaseCommonResources() {
  cameraRig_.Clear();
  ReleaseParticleEmitterManager();
}

Particle::Emitter::EmitterManager *Scene::GetParticleEmitterManager() {
  EnsureParticleEmitterManager();
  return particleEmitterManager_.get();
}

Atrum::Camera *Scene::GetCamera() { return cameraRig_.GetPrimaryCamera(this); }

const Atrum::Camera *Scene::GetCamera() const {
  return cameraRig_.GetPrimaryCamera(this);
}

void Scene::PrepareCamerasForDraw() {
  cameraRig_.PrepareActiveCameraBeforeDraw(*this);
}

void Scene::TickCameraRigEntities() {
  cameraRig_.TickSpawnedEntities();
}

void Scene::SetupStageViewCamera() {
  using namespace Atrum;

  Camera *camera = GetCamera();
  if (camera == nullptr) {
    return;
  }

  PublishStageViewFramingBounds(GetStageViewFramingBounds());
#if defined(DEVELOPMENT)
  const StageViewCameraPose pose =
      Editor::StageViewCameraTuning::Instance().IsGameViewPreviewEnabled()
          ? Editor::StageViewCameraTuning::Instance().ComputeBattlePose(
                GetStageViewFramingBounds())
          : ComputeStageBattleViewPose(GetStageViewFramingBounds());
#else
  const StageViewCameraPose pose =
      ComputeStageBattleViewPose(GetStageViewFramingBounds());
#endif
  ApplyStageViewCameraPose(camera, pose);
#if defined(DEVELOPMENT)
  Editor::StageViewCameraTuning &tuning =
      Editor::StageViewCameraTuning::Instance();
  if (tuning.IsGameViewPreviewEnabled() && tuning.IsLoaded()) {
    camera->SetPerspectiveFovMatrix(
        Math::Matrix4x4::PerspectiveFov(tuning.PerspectiveFovY(),
                                        tuning.AspectRatio(), 0.125f, 128.0f));
    camera->UpdateMatrix();
  }
#endif

#if defined(DEVELOPMENT) && !defined(NDEBUG)
  if (auto *debugCam = dynamic_cast<Debug::DebugCamera *>(camera)) {
    debugCam->RefPivot() = pose.pivot;
    debugCam->RefDistance() = std::abs(pose.eye.z - pose.pivot.z);
  }
#endif
}

void Scene::Draw() {
  Atrum::Engine *engine = Atrum::Engine::Instance();

  engine->PreDraw();
  {
#if defined(DEVELOPMENT)
    Atrum::Debug::SubsystemFrameScope sceneDrawScope(
        Atrum::Debug::SubsystemFrameId::GfxSceneDraw);
#endif
    DrawScene();
    Atrum::CommandChangeScene::Instance()->Draw();
#ifdef USE_IMGUI
    engine->ImGuiRender();
#endif
  }
  engine->PostDraw();
}

} // namespace Atrum
