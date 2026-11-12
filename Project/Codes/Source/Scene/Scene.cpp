#include "Scene/Scene.h"

#include "Camera/StageViewCamera.h"
#include "System/Entity/Entity.h"
#include "Engine/AtrumEngine.h"
#include "Scene/CommandChangeScene.h"

#include <cmath>

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

Atrum::Camera *Scene::GetCamera() { return cameraRig_.GetPrimaryCamera(); }

const Atrum::Camera *Scene::GetCamera() const {
  return cameraRig_.GetPrimaryCamera();
}

void Scene::PrepareCamerasForDraw() {
  cameraRig_.PrepareActiveCameraBeforeDraw(*this);
}

void Scene::SetupStageViewCamera() {
  using namespace Atrum;

  Camera *camera = GetCamera();
  if (camera == nullptr) {
    return;
  }

  const StageViewCameraPose pose = ComputeStageBattleViewPose();
  ApplyStageViewCameraPose(camera, pose);

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
  DrawScene();
  Atrum::CommandChangeScene::Instance()->Draw();
#ifdef USE_IMGUI
  engine->ImGuiRender();
#endif
  engine->PostDraw();
}

} // namespace Atrum
