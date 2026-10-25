#pragma once

#include "Camera/CameraController/TimedStageViewCameraController.h"
#include "Camera/StageViewCamera.h"
#include "Math/Vector3.h"

namespace Atrum {

class Camera;

/// 開始姿勢からワールド上の注視点へカメラを寄せる汎用演出（注視点は毎フレーム更新可能）
class CameraFocusPushController final : public TimedStageViewCameraController {
public:
  void Initialize() override {}

  /// 任意の開始姿勢から焦点へ pushDurationSeconds かけて寄せる
  void Begin(const StageViewCameraPose &startPose, float pushDurationSeconds,
             float subjectHalfHeight, float subjectHalfWidth);

  /// 戦闘全景カメラから焦点へ寄せる
  void BeginFromStageBattleView(float pushDurationSeconds,
                                float subjectHalfHeight,
                                float subjectHalfWidth);

  /// ボス撃破演出用（時間・被写体サイズは StageViewCamera の既定値）
  void BeginBossDefeatFromStageBattle(Camera *camera,
                                      const Math::Vector3 &initialFocusWorld);

  /// Update の直前に設定する（移動する被写体にも追従できる）
  void SetFocusWorldPosition(const Math::Vector3 &focusWorld);

  void Update() override;

  /// 寄せ演出の進行 [0,1]（完了後も 1.0）
  float GetPushProgress01() const;

  bool IsPushFinished() const;

private:
  float subjectHalfHeight_ = 1.0f;
  float subjectHalfWidth_ = 1.0f;
  Math::Vector3 focusWorld_{};
  StageViewCameraPose startPose_{};

  void ApplyCurrentPose() const;
};

} // namespace Atrum
