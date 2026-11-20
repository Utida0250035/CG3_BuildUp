#pragma once

#include "Audio/AudioHandle.h"
#include "Scene.h"
#include <memory>
#include <vector>

namespace Atrum {

class Entity;

/// <summary>
/// タイトルシーン
/// </summary>
class SceneTitle : public Scene {
public:
  SceneTitle();
  ~SceneTitle() override;

  void EnterScene() override;
  void Update() override;
  void ExitScene() override;

  /// <summary>
  /// シングルトンインスタンス
  /// </summary>
  static SceneTitle *Instance();

  bool IsStageViewCameraLocked() const override { return true; }

  bool TitleSubmitTransitionConsumed() const {
    return titleSubmitTransitionConsumed_;
  }
  void SetTitleSubmitTransitionConsumed(const bool consumed) {
    titleSubmitTransitionConsumed_ = consumed;
  }

protected:
  void DrawScene() override;

private:
  bool titleSubmitTransitionConsumed_ = false;
  Atrum::Audio::Handle sceneBgmHandle_{};
  Atrum::Audio::Handle sceneAmbientLoopSeHandle_{};
  std::unique_ptr<Atrum::Entity> skyDome_;
  std::vector<std::unique_ptr<Atrum::Entity>> backgrounds_;
  std::vector<std::unique_ptr<Atrum::Entity>> foregroundGroundClouds_;

  void ClearBackgrounds();
  void SpawnBackgrounds();
  void UpdateBackgrounds();
  void DrawBackgrounds();
};

} // namespace Atrum
