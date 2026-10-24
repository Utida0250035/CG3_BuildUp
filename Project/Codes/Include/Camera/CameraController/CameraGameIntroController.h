#pragma once

#include "Camera/CameraController/CameraController.h"
#include "Camera/CameraController/CameraPoseBlendController.h"
#include "Camera/StageViewCamera.h"
#include "Math/Vector3.h"

namespace Atrum {

/// GameScene 開始時：ボス中心の寄り → HP バー演出 → 戦闘全景へカメラを線形補間
class CameraGameIntroController final : public CameraController {
public:
  void Initialize() override;

  /// ボスワールド座標を基準にイントロを開始する（透視行列も設定）
  void BeginIntro(const Math::Vector3 &bossWorldPosition);

  void Update() override;

  /// ボス HP バーの伸び演出用 [0,1]
  float GetBossHpBarReveal01() const;

  bool IsFinished() const;

  /// 演出を打ち切り、戦闘全景カメラ・HP バー表示完了状態にする
  void SkipIntro();

private:
  enum class Phase : uint8_t { HpBarReveal, CameraPullBack, Finished };

  Phase phase_ = Phase::HpBarReveal;
  float phaseElapsedSeconds_ = 0.0f;

  StageViewCameraPose bossFocusPose_{};
  StageViewCameraPose stageBattlePose_{};
  CameraPoseBlendController pullBackBlend_{};

  static constexpr float kHpBarRevealDurationSeconds = 1.25f;
  static constexpr float kCameraPullBackDurationSeconds = 1.6f;

  void FinishIntro();
};

} // namespace Atrum
