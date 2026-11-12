#pragma once

#include "Scene.h"
#include "Math/Vector3.h"
#include <string>

namespace Atrum {

/// パーティクルサンプル自動巡回＋演出デモ（ボール壁衝突など）（Development + ImGui）
class SceneParticleDemo : public Scene {
public:
  SceneParticleDemo();
  ~SceneParticleDemo() override;

  void EnterScene() override;
  void Update() override;
  void ExitScene() override;

  static SceneParticleDemo *Instance();

  bool IsStageViewCameraLocked() const override { return false; }
  bool AllowDebugCameraControl() const override { return true; }

protected:
  void DrawScene() override;

private:
  void SetupDemoCamera();
  void BeginStep(int stepIndex);
  void AdvanceStep(int delta);
  void UpdateShowcaseStep(float deltaSeconds);
  void UpdateBallArenaStep(float deltaSeconds);
  void ClearShowcaseEmitter();
  void SpawnWallImpact(const Math::Vector3 &worldPosition);
  void DrawArenaProps();
  void DrawDemoBall();

  int stepIndex_ = 0;
  float stepElapsedSeconds_ = 0.0f;
  bool autoAdvance_ = true;
  uint32_t impactSequence_ = 0;

  static constexpr const char *kShowcaseEmitterName = "particle_demo_showcase";

  Math::Vector3 ballPosition_{0.0f, 0.0f, 0.0f};
  Math::Vector3 ballVelocity_{7.0f, 5.5f, 0.0f};
  float ballRadius_ = 0.32f;
  float wallImpactCooldownSeconds_ = 0.0f;

  std::string impactJsonPath_{};
};

} // namespace Atrum
