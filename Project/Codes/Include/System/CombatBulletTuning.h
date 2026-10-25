#pragma once

#include "Math/Vector3.h"

namespace Atrum {

/// 敵弾・反射弾の速度に共通で掛ける倍率（全体的な難易度調整用）
inline constexpr float kBulletSpeedGlobalScale = 1.3f;

inline Math::Vector3 ScaleBulletVelocity(const Math::Vector3 &velocity) {
  return velocity * kBulletSpeedGlobalScale;
}

} // namespace Atrum
