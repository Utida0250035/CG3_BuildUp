#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpAtomFEnemyKnockback;
class CmpParticleEmitter;

/// atomF 敵が吹き飛んでいる間だけ、子エンティティのパーティクルを発生させる。
/// - 後方噴射（吹き飛び方向と反対へ噴き出す。発生時は黄色→消失までに赤）
/// - 纏い（敵の周囲に出る黄色）
class CmpAtomFEnemyKnockbackFx final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PRE_RENDER;
  }

  void ResolveDependence() override;
  void Update() override;
  /// 子エンティティはシーンから Finalize されないため、エミッタ解除をここで行う。
  void Finalize() override;

  void SetTrailEmitter(CmpParticleEmitter *emitter) { trailEmitter_ = emitter; }
  void SetAuraEmitter(CmpParticleEmitter *emitter) { auraEmitter_ = emitter; }

private:
  void SetEmitting(const bool emitting);

  CmpAtomFEnemyKnockback *knockback_ = nullptr;
  CmpParticleEmitter *trailEmitter_ = nullptr;
  CmpParticleEmitter *auraEmitter_ = nullptr;
  bool emitting_ = false;
};

} // namespace Atrum
