#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 床を外側へ広がる衝撃波。描画は同じエンティティの CmpDrawAtomFTelegraph が担う。
class CmpAtomFBossShockwave final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_RESPONSE;
  }

  void ResolveDependence() override;
  void Update() override;

  void Setup(const Math::Vector3 &center, const float maxRadius,
             const float speed, const float bandHalfWidth) {
    center_ = center;
    maxRadius_ = maxRadius;
    speed_ = speed;
    bandHalfWidth_ = bandHalfWidth;
  }

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Math::Vector3 center_{};
  float radius_ = 0.0f;
  float maxRadius_ = 6.0f;
  float speed_ = 5.0f;
  float bandHalfWidth_ = 0.3f;
  bool hasHitPlayer_ = false;
};

} // namespace Atrum
