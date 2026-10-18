#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Atrum::Visual {

enum class ParticleEmitMode { BURST, CONTINUOUS };

enum class ParticleEmitShape { Point, Circle, Box };

enum class ParticleMovementMode { DIRECTIONAL, RADIAL_SPREAD, RADIAL_CONVERGE };

enum class ParticleKillFieldMode { OUTSIDE, INSIDE };

enum class ParticleKillFieldShape { SPHERE, AABB };

struct ParticleKillFieldParam {
  bool isUse = false;
  ParticleKillFieldMode mode = ParticleKillFieldMode::INSIDE;
  ParticleKillFieldShape shape = ParticleKillFieldShape::SPHERE;
  float localSphereRadius = 0.0f;
  Math::Vector3 localAabbMin{};
  Math::Vector3 localAabbMax{};

  bool isFollowsEmitter = true;
  Math::Vector3 localOffset{};
  Math::Vector3 worldPos{};
};

struct ParticleParam {

  ParticleEmitMode emitMode = ParticleEmitMode::BURST;

  ParticleEmitShape emitShape = ParticleEmitShape::Point;
  float emitSphereRadius = 0.0f;
  Math::Vector3 emitAabbMin{};
  Math::Vector3 emitAabbMax{};

  ParticleMovementMode movementMode = ParticleMovementMode::DIRECTIONAL;

  // CONTINUOUS用 1秒間の発射数(1/emitRate[s]につき1つ)
  float emitRate = 0.0f;
  // BURST用 1回の発射数
  size_t burstCount = 0;
  // CONTINUOUS用 エミッターの寿命を無くして発生をループさせるかどうか
  bool isLooping = false;
  // CONTINUOUS用 エミッターの寿命[s]
  float emitterLifeTime = 0.0f;

  // 生存時間 ランダム幅
  float minLife = 0.0f;
  float maxLife = 0.0f;

  // 速度と拡散・加減速
  Math::Vector3 baseVelocity = {0.0f, 0.0f, 0.0f};
  Math::Vector3 velocityRandom = {0.0f, 0.0f, 0.0f}; // ランダムの振れ幅
  Math::Vector3 acceleration = {0.0f, 0.0f, 0.0f};   // 重力など
  float damping = 0.0f;                              // 空気抵抗（減衰）

  // スケール変化
  float startScale = 0.0f;
  float endScale = 0.0f;

  // カラー変化（発生時 〜 消失時）
  Math::Vector4 startColor = {0.0f, 0.0f, 0.0f, 0.0f};
  Math::Vector4 endColor = {0.0f, 0.0f, 0.0f, 0.0f};

  // キルフィールド設定
  ParticleKillFieldParam killFieldParam{};
};

} // namespace Atrum::Visual