#pragma once

#include "Math/Vector4.h"
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

private:
  void BeginDyingExplosion();
  void UpdateDying();

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  bool isDying_ = false;
  float shrinkElapsedSeconds_ = 0.0f;
  float shrinkDurationSeconds_ = 0.6f;
  float initialMeshRadius_ = 0.25f;
  /// 縮小消滅中に使う元の色（種類ごとの色を保つ）。
  Math::Vector4 dyingBaseColor_{0.92f, 0.28f, 0.25f, 1.0f};
};

} // namespace Atrum