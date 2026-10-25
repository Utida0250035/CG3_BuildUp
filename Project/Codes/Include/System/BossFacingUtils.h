#pragma once

#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

/// ボスを自機方向へ向ける（XY 平面・Z 軸回り。オービター公転面も自機側を向く）
inline void ApplyBossFacingTowardPlayer(Entity &boss, const Entity &player) {
  Math::Vector3 toPlayer = player.GetWorldPosition() - boss.GetWorldPosition();
  toPlayer.z = 0.0f;
  if (toPlayer.LengthSquare() <= 1.0e-10f) {
    return;
  }

  const float yawRadians = std::atan2(toPlayer.y, toPlayer.x);
  boss.RefTransform().quaternion =
      Math::Quaternion::FromAxisAngle(Math::Vector3::ForwardLh(), yawRadians);
}

} // namespace Atrum
