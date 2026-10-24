#include "Scene/Scene.h"

#include "Camera/StageViewCamera.h"
#include "Engine/AtrumEngine.h"
#include "Scene/CommandChangeScene.h"

#include <cmath>

// Release（NDEBUG）では DebugCamera を使わない
#if defined(DEVELOPMENT) && !defined(NDEBUG)
#include "ForDebug/DebugCamera.h"
#endif

Scene::Scene() { InitializeCamera(); }

Scene::~Scene() = default;

void Scene::InitializeCamera() {
#if defined(DEVELOPMENT) && !defined(NDEBUG)
  camera_ = std::make_unique<Atrum::Debug::DebugCamera>();
#else
  camera_ = std::make_unique<Atrum::Camera>();
#endif

  camera_->Initialize();
  camera_->CreateOrthographicMatrix(Atrum::Window::kDefaultClientWidth,
                                    Atrum::Window::kDefaultClientHeight);
}

void Scene::SetupStageViewCamera() {
  using namespace Atrum;

  const StageViewCameraPose pose = ComputeStageBattleViewPose();
  ApplyStageViewCameraPose(camera_.get(), pose);

#if defined(DEVELOPMENT) && !defined(NDEBUG)
  if (auto *debugCam =
          dynamic_cast<Debug::DebugCamera *>(camera_.get())) {
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
