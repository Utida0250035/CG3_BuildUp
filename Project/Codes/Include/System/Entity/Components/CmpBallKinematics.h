#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// 重力と速度でオーナー位置を積分する（軸バウンス・演出とは分離）
class CmpBallKinematics : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PHYSICS;
  }

  void Initialize() override;
  void Update() override;

  void SetInitialVelocity(const Math::Vector3 &velocity) {
    velocity_ = velocity;
  }
  void SetGravity(const float gravity) { gravity_ = gravity; }

  Math::Vector3 &RefVelocity() { return velocity_; }
  Math::Vector3 GetVelocity() const { return velocity_; }

private:
  Math::Vector3 velocity_{7.0f, 5.5f, 0.0f};
  float gravity_ = -14.0f;
};

} // namespace Atrum