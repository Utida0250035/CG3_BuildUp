#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 雑魚の射撃型。一定間隔で立ち止まり（予備動作: 発光しながら狙いを定める）、自機へ弾を 1 発撃つ。
/// variant の shootIntervalSeconds が 0 以下なら何もしない。Hp1・耐久なしは他の雑魚と同じ。
class CmpAtomFEnemyShooter final : public ComponentUpd {
public:
  enum class Phase { Waiting, Aiming };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  bool IsEnabled() const { return shootIntervalSeconds_ > 0.0f; }
  bool BlocksLocomotion() const { return phase_ == Phase::Aiming; }
  Phase CurrentPhase() const { return phase_; }
  /// 狙っている向き（XZ 単位ベクトル）。Aiming 中のみ有効。
  const Math::Vector3 &AimDirection() const { return aimDirection_; }
  /// 予備動作の進行度 0〜1。
  float AimProgress() const;

private:
  void FireProjectile();
  void ApplyAimVisual(bool aiming);

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Phase phase_ = Phase::Waiting;
  float shootIntervalSeconds_ = 0.0f;
  float waitTimerSeconds_ = 0.0f;
  float aimElapsedSeconds_ = 0.0f;
  float baseColorR_ = 1.0f;
  float baseColorG_ = 1.0f;
  float baseColorB_ = 1.0f;
  float baseRadius_ = 0.25f;
  Math::Vector3 aimDirection_{0.0f, 0.0f, 1.0f};
};

} // namespace Atrum
