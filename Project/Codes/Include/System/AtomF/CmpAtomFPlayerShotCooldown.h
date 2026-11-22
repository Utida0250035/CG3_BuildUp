#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 自機の射撃間隔。parameters.json の shotCooldownSeconds を空ける。
class CmpAtomFPlayerShotCooldown final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_JUDGE;
  }

  void ResolveDependence() override;
  void Update() override;

  float GetShotCooldownSeconds() const;

  bool IsShotReady() const { return cooldownRemainingSeconds_ <= 0.0f; }

  /// 発射を許可できるならクールタイムを開始し true を返す。
  bool TryConsumeShotCooldown();

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  float cooldownRemainingSeconds_ = 0.0f;
};

} // namespace Atrum