#pragma once

#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 雑魚敵の種類（歩行速度・蛇行・突進の有無）を保持するデータコンポーネント。
/// 全種 Hp1・耐久系能力なし。挙動は CmpAtomFEnemyWalkToPlayer / PeriodicCharge が参照する。
class CmpAtomFEnemyVariant final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override {}
  void Update() override {}

  void SetParameters(const AtomF::EnemyVariantParameters &parameters) {
    parameters_ = parameters;
  }
  const AtomF::EnemyVariantParameters &Parameters() const {
    return parameters_;
  }

private:
  AtomF::EnemyVariantParameters parameters_{};
};

} // namespace Atrum
