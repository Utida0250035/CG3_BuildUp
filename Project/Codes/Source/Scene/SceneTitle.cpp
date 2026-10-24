#include "Scene/SceneTitle.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/LightData.h"
#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneGame.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"

namespace {

void SetupStageLighting() {
  Atrum::Math::Vector3 lightDirection{-0.4f, -0.85f, 0.15f};
  lightDirection.Normalize();
  const Atrum::DirectionalLightData directionalLight{
      .color = Atrum::Math::Vector3{1.0f, 1.0f, 1.0f},
      .direction = lightDirection,
      .intensity = 8.0f,
      .lightModel = Atrum::LightModel::HalfLambert,
  };
  Atrum::Engine::Instance()->SetDirectionalLightData(directionalLight);
}

} // namespace

void SceneTitle::EnterScene() {
  SetupStageViewCamera();
  SpawnBackgrounds();
}

void SceneTitle::Update() {
  UpdateBackgrounds();

  if (Atrum::AbstractInput::Instance()->GetData().isSubmitTrigger) {
    Atrum::CommandChangeScene::Instance()->Set(SceneGame::Instance(),
                                               new Atrum::ChangeSceneFade());
  }
}

void SceneTitle::DrawScene() {
  SetupStageLighting();
  DrawBackgrounds();
}

void SceneTitle::ExitScene() { ClearBackgrounds(); }

void SceneTitle::SpawnBackgrounds() {
  ClearBackgrounds();
  skyDome_ = Atrum::EntityFactory::SkyDome(GetCamera());
  backgrounds_ = Atrum::EntityFactory::ScrollSpheres();
  foregroundGroundClouds_ = Atrum::EntityFactory::ForegroundGroundClouds();
}

void SceneTitle::ClearBackgrounds() {
  if (skyDome_) {
    skyDome_->Finalize();
    skyDome_.reset();
  }

  for (std::unique_ptr<Atrum::Entity> &background : backgrounds_) {
    if (background) {
      background->Finalize();
    }
  }
  backgrounds_.clear();

  for (std::unique_ptr<Atrum::Entity> &cloud : foregroundGroundClouds_) {
    if (cloud) {
      cloud->Finalize();
    }
  }
  foregroundGroundClouds_.clear();
}

void SceneTitle::UpdateBackgrounds() {
  for (std::unique_ptr<Atrum::Entity> &background : backgrounds_) {
    if (background) {
      background->Update();
    }
  }
}

void SceneTitle::DrawBackgrounds() {
  if (skyDome_) {
    skyDome_->Draw();
  }

  for (const std::unique_ptr<Atrum::Entity> &background : backgrounds_) {
    if (background) {
      background->Draw();
    }
  }

  for (const std::unique_ptr<Atrum::Entity> &cloud : foregroundGroundClouds_) {
    if (cloud) {
      cloud->Draw();
    }
  }
}

SceneTitle::SceneTitle() {}

SceneTitle *SceneTitle::Instance() {
  static SceneTitle instance;
  return &instance;
}
