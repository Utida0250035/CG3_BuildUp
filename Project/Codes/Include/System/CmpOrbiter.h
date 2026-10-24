#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class FrameDeltaTime;

class CmpOrbiter : public ComponentUpd {
private:
  Math::Vector3 rotateAxis_ = Math::Vector3::UpLh();
  float rotateSpeed_ = 0.0f;
  FrameDeltaTime *frameDeltaTime_ = nullptr;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::MOVEMENT; }

  void ResolveDependence() override;
  void Update() override;

  void SetRotateAxis(const Math::Vector3 &axis) { rotateAxis_ = axis; }
  void SetRotateSpeed(const float rotateSpeed) { rotateSpeed_ = rotateSpeed; }

  /// 地面薙ぎ払い前の状態を保存する
  void CaptureDefaults();
  /// CaptureDefaults 以前の回転設定と位置半径へ戻す
  void RestoreDefaults();
  /// 通常軌道から薙ぎ払い準備位置（XZ 半径 hubRadius）へ球面補間で移動
  void BeginMoveToSweepStart(float localOrbitHeightY, float hubRadius,
                             float startAngleRadians);
  void SetMoveToSweepStartProgress(float progress01);
  void EndMoveToSweepStart();

  /// 薙ぎ払い後、CaptureDefaults 時の位置へ球面補間で集める
  void BeginGatherToDefaultPosition();
  void SetGatherToDefaultProgress(float progress01);
  void EndGatherToDefaultPosition();
  /// 集め先（CaptureDefaults 時のローカル位置）のワールド座標
  Math::Vector3 GetGatherTargetWorldPosition();

  /// ボス真下付近で半径を固定したまま素早く回転（薙ぎ払い半径拡大の前）
  void BeginSpinUnderBoss(float rotateSpeed, float localOrbitHeightY,
                          float startAngleRadians, float hubRadius);
  void EndSpinUnderBoss();

  /// 地面薙ぎ払い（半径拡大）。hubRadius から maxOrbitRadius へ補間
  void BeginGroundSweep(float rotateSpeed, float maxOrbitRadius,
                        float localOrbitHeightY, float startAngleRadians,
                        float hubRadius = 0.0f);
  void BeginGroundSweepExpandFromSpin(float expandRotateSpeed,
                                      float maxOrbitRadius);
  void SetGroundSweepRadiusScale(float scale01);
  void EndGroundSweep();

  /// 集め直線軌道を確定させ、移動開始まで位置・公転を止める
  void BeginGatherTrajectoryHold();
  void EndGatherTrajectoryHold();

private:
  Math::Vector3 defaultAxis_{};
  float defaultSpeed_ = 0.0f;
  Math::Vector3 defaultTranslate_{};
  bool hasDefaults_ = false;

  bool isMoveToSweepStartActive_ = false;
  bool isGatherToDefaultActive_ = false;
  bool isGatherTrajectoryHoldActive_ = false;
  Math::Vector3 moveStartTranslate_{};
  Math::Vector3 moveTargetTranslate_{};

  bool isSpinUnderBossActive_ = false;
  float spinHubRadius_ = 0.0f;

  bool isGroundSweepActive_ = false;
  float sweepMaxRadius_ = 0.0f;
  float sweepHubRadius_ = 0.0f;
  float sweepLocalY_ = 0.0f;
  float sweepAngleRadians_ = 0.0f;
  float sweepRotateSpeed_ = 0.0f;
  float sweepRadiusScale_ = 0.0f;
};

} // namespace Atrum
