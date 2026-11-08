#pragma once

#include "Camera/CameraController/MovieCameraController.h"
#include "Scene.h"

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Entity;
class Scene;

/// JSON トラックを再生するムービーシーン（非シングルトン）
class SceneMovie final : public Scene {
public:
  SceneMovie(std::string trackJsonPath, Scene *returnScene);

  void EnterScene() override;
  void Update() override;
  void ExitScene() override;

  bool IsStageViewCameraLocked() const override { return true; }

protected:
  void DrawScene() override;

private:
  std::string trackJsonPath_{};
  Scene *returnScene_ = nullptr;
  MovieCameraController movieCamera_{};
  bool trackPlaybackStarted_ = false;
  bool hasPendingSceneChange_ = false;

  std::unique_ptr<Entity> skyDome_{};

  void ClearVisuals();
  void SpawnVisuals();
  void SetupStageLighting();
  void RequestReturnToCaller();
};

class SceneMovieRegistry final {
public:
  static SceneMovieRegistry &Instance();

  Scene *Launch(const std::string &trackJsonPath, Scene *returnScene);

  void OnSceneExited(SceneMovie *scene);

private:
  std::vector<std::unique_ptr<SceneMovie>> ownedScenes_{};
};

} // namespace Atrum
