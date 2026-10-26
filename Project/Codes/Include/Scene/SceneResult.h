#pragma once

#include "Audio/AudioHandle.h"
#include "Scene.h"
#include <memory>
#include <vector>

namespace Atrum {

class Entity;

/// <summary>
/// リザルトシーン
/// </summary>
class SceneResult : public Scene {
public:
  SceneResult();
  ~SceneResult() override = default;

  void EnterScene() override;
  void Update() override;
  void ExitScene() override;

  /// <summary>
  /// シングルトンインスタンス
  /// </summary>
  static SceneResult *Instance();

  bool IsStageViewCameraLocked() const override { return true; }

protected:
  void DrawScene() override;

private:
  Atrum::Audio::Handle sceneBgmHandle_{};
  std::unique_ptr<Atrum::Entity> skyDome_;
  std::vector<std::unique_ptr<Atrum::Entity>> backgrounds_;
  std::vector<std::unique_ptr<Atrum::Entity>> foregroundGroundClouds_;

  void ClearBackgrounds();
  void SpawnBackgrounds();
  void UpdateBackgrounds();
  void DrawBackgrounds();
};

} // namespace Atrum
