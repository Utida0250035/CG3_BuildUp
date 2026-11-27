#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 自機弾ノックバック後、壁や別敵への衝突で爆発して縮小消滅する。
class CmpAtomFEnemyChainExplosion final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_RESPONSE;
  }

  void ResolveDependence() override;
  void Update() override;

  bool IsDying() const { return isDying_; }

  /// ボス撃破時の一掃用。爆発の見た目だけ出して縮小消滅し、自機へのダメージ・他敵への吹き飛ばしは行わない。
  void BeginSilentDefeat();

private:
  void BeginDyingExplosion();
  void UpdateDying();

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  bool isDying_ = false;
  float shrinkElapsedSeconds_ = 0.0f;
  float shrinkDurationSeconds_ = 0.6f;
  float initialMeshRadius_ = 0.25f;
};

} // namespace Atrum