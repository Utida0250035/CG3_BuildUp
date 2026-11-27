#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 雑魚の出現演出。出現直後の短い間、メッシュが小さい状態から弾むように膨らむ。
/// 当たり判定は変えない（見た目だけ）。
class CmpAtomFEnemySpawnIn final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::ANIMATION;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  float elapsedSeconds_ = 0.0f;
  float baseRadius_ = 0.25f;
  bool finished_ = false;
};

} // namespace Atrum
