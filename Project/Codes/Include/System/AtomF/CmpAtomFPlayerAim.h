#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 自機の XZ エイム角度（ラジアン）。マウスと R スティック。
class CmpAtomFPlayerAim final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::INPUT;
  }

  void Update() override;

  float GetAimAngleRadians() const { return aimAngleRadians_; }

  /// XZ 平面上の単位方向（Y=0）。
  Math::Vector3 GetAimDirectionXZ() const;

  /// 今フレームでマウスまたは R スティックからエイム入力があった。
  bool IsAimInputActive() const { return aimInputActive_; }

private:
  float aimAngleRadians_ = 0.0f;
  bool aimInputActive_ = false;
};

} // namespace Atrum