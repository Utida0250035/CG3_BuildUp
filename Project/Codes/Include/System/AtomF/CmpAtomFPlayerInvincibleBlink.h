#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpHealthHit;
class CmpMeshSphere;

/// atomF 自機の無敵中の点滅。無敵開始(0秒)から blinkPulseSeconds をかけて薄くなる周期を
/// blinkPulseCount 回繰り返し、残りの無敵時間は演出なし。
class CmpAtomFPlayerInvincibleBlink final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::ANIMATION;
  }

  void ResolveDependence() override;
  void Update() override;

  /// 無敵開始からの経過秒に対する不透明度（演出なしの区間は 1.0）。
  static float EvaluateAlpha(float elapsedSeconds, float pulseSeconds,
                             int pulseCount, float minAlpha);

private:
  CmpHealthHit *health_ = nullptr;
  CmpMeshSphere *mesh_ = nullptr;
};

} // namespace Atrum
