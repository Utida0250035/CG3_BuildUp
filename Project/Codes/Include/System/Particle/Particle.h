#pragma once

#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Atrum::Visual::Particle {

struct Particle {

  /* 空間・移動 */

  // 現在の座標
  Math::Vector3 position{};
  // 現在の速度
  Math::Vector3 velocity{};
  // 加速度
  Math::Vector3 acceleration{};
  // 速度減衰係数
  float damping = 0.0f;

  // 3D回転
  Math::Quaternion rotation{};
  // 毎フレームの回転スピード
  Math::Vector3 angularVelocity{};

  /* 見た目の補間用 */

  // 発生時の色(RGBA)
  Math::Vector4 startColor{};
  // 消滅時の色(RGBA)
  Math::Vector4 endColor{};
  // 現在の色(RGBA)
  Math::Vector4 currentColor{};

  // 発生時の大きさ
  Math::Vector3 startScale{};
  // 消滅時の大きさ
  Math::Vector3 endScale{};
  // 現在の大きさ
  Math::Vector3 currentScale{};

  /* 寿命データ */

  // 残り寿命
  float lifeTimePassed = 0.0f;

  // 寿命設定
  float lifeDuration = 0.0f;

  /* 状態管理 */

  // 有効フラグ falseなら非アクティブ/プール再利用待ち
  bool isActive = false;
};

using Data = Particle;

} // namespace Atrum::Visual::Particle