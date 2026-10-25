#include "Scene/SceneResult.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/LightData.h"
#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneTitle.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "Scene/SceneScreenUi.h"
#include "System/PlaySe.h"

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

void SceneResult::EnterScene() {
  SetupStageViewCamera();
  Atrum::SceneScreenUi::PreloadResultUiTextures();
  SpawnBackgrounds();
  sceneBgmHandle_ = Atrum::PlayBgmLoop({"./Assets/Audios/bgmResult.mp3",
                                        "./Resources/bgmResult.mp3"});
}

void SceneResult::Update() {
  UpdateBackgrounds();

  if (Atrum::AbstractInput::Instance()->GetData().isSubmitTrigger) {
    Atrum::PlaySubmitSe();
    Atrum::CommandChangeScene::Instance()->Set(SceneTitle::Instance(),
                                               new Atrum::ChangeSceneFade());
  }
}

void SceneResult::DrawScene() {
  SetupStageLighting();
  DrawBackgrounds();

  if (Atrum::AtrumEngine *engine = Atrum::AtrumEngine::Instance()) {
    Atrum::SceneScreenUi::DrawResultSceneUi(engine, GetCamera());
  }
}

void SceneResult::ExitScene() {
  Atrum::StopBgmLoop(sceneBgmHandle_);
  sceneBgmHandle_ = {};
  ClearBackgrounds();
}

void SceneResult::SpawnBackgrounds() {
  ClearBackgrounds();
  skyDome_ = Atrum::EntityFactory::SkyDome(GetCamera());
  backgrounds_ = Atrum::EntityFactory::ScrollSpheres();
  foregroundGroundClouds_ = Atrum::EntityFactory::ForegroundGroundClouds();
}

void SceneResult::ClearBackgrounds() {
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

void SceneResult::UpdateBackgrounds() {
  for (std::unique_ptr<Atrum::Entity> &background : backgrounds_) {
    if (background) {
      background->Update();
    }
  }
}

void SceneResult::DrawBackgrounds() {
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

SceneResult::SceneResult() {}

SceneResult *SceneResult::Instance() {
  static SceneResult instance;
  return &instance;
}
