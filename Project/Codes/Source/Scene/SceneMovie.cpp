#include "Scene/SceneMovie.h"

#include "Scene/SceneMovieReturnTarget.h"
#include "Scene/SceneMovieTrackFailure.h"
#include "Scene/SceneMovieVisualSetup.h"
#include "Scene/SceneMovieAudioPolicy.h"
#include "Scene/SceneMoviePendingTransitionUpdate.h"
#include "Scene/SceneMovieStageLighting.h"
#include "System/DeltaTime.h"
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "imgui.h"
#endif

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
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

void SceneMovie::ClearVisuals() {
  if (skyDome_) {
    skyDome_->Finalize();
    skyDome_.reset();
  }
}

void SceneMovie::SpawnVisuals() {
  ClearVisuals();
  skyDome_ = CreateSkyDomeBoundToCamera(GetCamera());
}

void SceneMovie::RequestReturnToCaller() {
  if (hasPendingSceneChange_) {
    return;
  }

  Scene *const target = ResolveSceneMovieReturnTarget(returnScene_);
  SceneMovieAudioPolicy::PrepareReturnSceneBgmBeforeFade(target);
  hasPendingSceneChange_ = true;
  CommandChangeScene::Instance()->Set(
      target, std::make_unique<ChangeSceneFade>().release());
}

void SceneMovie::EnterScene() {
  EnsureCommonResources();
  hasPendingSceneChange_ = false;
  trackPlaybackStarted_ = false;
  trackLoadFailed_ = false;
  trackLoadFailureElapsedSeconds_ = 0.0f;
  trackLoadFailureMessage_.clear();

  ApplySceneMovieStageLighting();
  SceneMovieAudioPolicy::OnEnterScene();

  movieCamera_.Initialize();
  movieCamera_.SetCamera(GetCamera());
  trackPlaybackStarted_ =
      movieCamera_.TryBeginFromJsonFile(trackJsonPath_);

  if (!trackPlaybackStarted_) {
    trackLoadFailed_ = true;
    trackLoadFailureMessage_ =
        BuildSceneMovieTrackLoadFailureMessage(trackJsonPath_);
  }

  SpawnVisuals();
}

void SceneMovie::Update() {
  SceneMovieAudioPolicy::TickDuringMovie(hasPendingSceneChange_);
  if (hasPendingSceneChange_) {
    TickSceneMovieVisualsDuringPendingTransition(
        trackPlaybackStarted_, movieCamera_, skyDome_.get());
    return;
  }

  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Input::Key::ESCAPE)) {
    RequestReturnToCaller();
    return;
  }

  if (trackLoadFailed_) {
    trackLoadFailureElapsedSeconds_ +=
        ::Atrum::EngineFrameDeltaTime()->GetDeltaTime();
    if (trackLoadFailureElapsedSeconds_ >=
        kSceneMovieTrackLoadFailureHoldSeconds) {
      RequestReturnToCaller();
    }
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

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (trackLoadFailed_ && !trackLoadFailureMessage_.empty()) {
    ImGui::SetNextWindowPos(ImVec2(24.0f, 24.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(520.0f, 180.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("ムービートラック（読込失敗）", nullptr,
                     ImGuiWindowFlags_NoCollapse)) {
      ImGui::TextWrapped("%s", trackLoadFailureMessage_.c_str());
      const float remaining =
          kSceneMovieTrackLoadFailureHoldSeconds -
          trackLoadFailureElapsedSeconds_;
      ImGui::TextDisabled("自動復帰まで: %.1f 秒", std::max(0.0f, remaining));
    }
    ImGui::End();
  }
#endif
}

void SceneMovie::ExitScene() {
  SceneMovieAudioPolicy::OnExitScene();
  ClearVisuals();
  movieCamera_.Finalize();
  SceneMovieRegistry::Instance().OnSceneExited(this);
}

} // namespace Atrum
