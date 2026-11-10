#include "Scene/SceneMovie.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/LightData.h"
#include "Input/AbstractInput.h"
#include "Input/PlayInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "System/EntityFactory.h"
#include "System/Entity/Entity.h"

#include <algorithm>

namespace Atrum {

SceneMovie::SceneMovie(std::string trackJsonPath, Scene *returnScene)
    : trackJsonPath_(std::move(trackJsonPath)), returnScene_(returnScene) {}

SceneMovie::~SceneMovie() = default;

SceneMovieRegistry::~SceneMovieRegistry() = default;

SceneMovieRegistry &SceneMovieRegistry::Instance() {
  static SceneMovieRegistry instance;
  return instance;
}

Scene *SceneMovieRegistry::Launch(const std::string &trackJsonPath,
                                  Scene *returnScene) {
  auto scene = std::make_unique<SceneMovie>(trackJsonPath, returnScene);
  Scene *raw = scene.get();
  ownedScenes_.push_back(std::move(scene));
  return raw;
}

void SceneMovieRegistry::OnSceneExited(SceneMovie *scene) {
  if (scene == nullptr) {
    return;
  }
  ownedScenes_.erase(
      std::remove_if(ownedScenes_.begin(), ownedScenes_.end(),
                     [scene](const std::unique_ptr<SceneMovie> &owned) {
                       return owned.get() == scene;
                     }),
      ownedScenes_.end());
}

void SceneMovie::SetupStageLighting() {
  Math::Vector3 lightDirection{-0.4f, -0.85f, 0.15f};
  lightDirection.Normalize();
  const DirectionalLightData directionalLight{
      .color = Math::Vector3{1.0f, 1.0f, 1.0f},
      .direction = lightDirection,
      .intensity = 8.0f,
      .lightModel = LightModel::HalfLambert,
  };
  Engine::Instance()->SetDirectionalLightData(directionalLight);
}

void SceneMovie::ClearVisuals() {
  if (skyDome_) {
    skyDome_->Finalize();
    skyDome_.reset();
  }
}

void SceneMovie::SpawnVisuals() {
  ClearVisuals();
  skyDome_ = EntityFactory::SkyDome(GetCamera());
}

void SceneMovie::RequestReturnToCaller() {
  if (hasPendingSceneChange_ || returnScene_ == nullptr) {
    return;
  }

  hasPendingSceneChange_ = true;
  CommandChangeScene::Instance()->Set(
      returnScene_, std::make_unique<ChangeSceneFade>().release());
}

void SceneMovie::EnterScene() {
  EnsureCommonResources();
  hasPendingSceneChange_ = false;
  trackPlaybackStarted_ = false;

  SpawnVisuals();
  SetupStageLighting();

  movieCamera_.Initialize();
  movieCamera_.SetCamera(GetCamera());
  trackPlaybackStarted_ =
      movieCamera_.TryBeginFromJsonFile(trackJsonPath_);

  if (!trackPlaybackStarted_) {
    SetupStageViewCamera();
  }
}

void SceneMovie::Update() {
  if (hasPendingSceneChange_) {
    return;
  }

  if (returnScene_ != nullptr &&
      ::Atrum::EnginePlayInput()->IsKeyTrigger(Input::Key::ESCAPE)) {
    RequestReturnToCaller();
    return;
  }

  if (!trackPlaybackStarted_) {
    RequestReturnToCaller();
    return;
  }

  movieCamera_.Update();

  if (skyDome_) {
    skyDome_->Update();
  }

  if (movieCamera_.IsFinished()) {
    RequestReturnToCaller();
  }
}

void SceneMovie::DrawScene() {
  if (skyDome_) {
    skyDome_->Draw();
  }
}

void SceneMovie::ExitScene() {
  ClearVisuals();
  movieCamera_.Finalize();
  SceneMovieRegistry::Instance().OnSceneExited(this);
}

} // namespace Atrum
