#pragma once

#include "Geometry/AABB.h"
#include "Geometry/Sphere.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/SimulationSpace.h"
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace Atrum::Visual::Particle {

enum class EmitMode { BURST, CONTINUOUS };

enum class EmitShape { POINT, CIRCLE_XZ, CIRCLE_XY, CIRCLE_YZ, AABB, SPHERE };

enum class MovementMode { DIRECTIONAL, RADIAL_SPREAD, RADIAL_CONVERGE };

enum class KillFieldMode { OUTSIDE, INSIDE };

enum class KillFieldShape { SPHERE, AABB };

struct KillFieldParam {
  bool isUse = false;
  KillFieldMode mode = KillFieldMode::INSIDE;
  KillFieldShape shape = KillFieldShape::SPHERE;
  Geometry::Sphere localSphere{};
  Geometry::AABB localAabb{};
  SimulationSpace simulationSpaceFromEmitter = SimulationSpace::LOCAL;
  Math::Vector3 position{};
};

void to_json(nlohmann::json &j, const KillFieldParam &killFieldParam);
void from_json(const nlohmann::json &j, KillFieldParam &killFieldParam);

struct EmitCircle {
  Math::Vector3 center{};
  float radius = 0.0f;
};

void to_json(nlohmann::json &j, const EmitCircle &emitCircle);
void from_json(const nlohmann::json &j, EmitCircle &emitCircle);

struct Param {

  SimulationSpace simulationSpaceFromEmitter = SimulationSpace::WORLD;

  EmitMode emitMode = EmitMode::CONTINUOUS;

  // 形状選択
  EmitShape emitShape = EmitShape::SPHERE;

  // 球データ
  Geometry::Sphere emitSphere{{}, 8.0f};

  // AABBデータ
  Geometry::AABB emitAabb{};

  // 円データ
  EmitCircle emitCircle{};

  // 点データ
  Math::Vector3 emitPointPos{};

  // 運動選択
  MovementMode movementMode = MovementMode::DIRECTIONAL;

  // CONTINUOUS用 1秒間の発射数(1/emitRate[s]につき1つ)
  float emitRate = 5.0f;
  // BURST用 1回の発射数
  size_t burstCount = 0;
  // CONTINUOUS用 断続時間を無くして発生をループさせるかどうか
  bool isLooping = false;
  // CONTINUOUS用 発生断続時間[s]
  float duration = 20.0f;

  // 生存時間 ランダム幅
  float minLifeDuration = 0.5f;
  float maxLifeDuration = 1.0f;

  // 速度と拡散・加減速
  Math::Vector3 baseVelocity = {0.0f, 0.0f, 0.0f};
  Math::Vector3 velocityRandom = {0.0f, 0.0f, 0.0f}; // ランダムの振れ幅
  Math::Vector3 acceleration = {0.0f, 0.0f, 0.0f};   // 重力など
  float damping = 0.0f;                              // 空気抵抗（減衰）

  // スケール変化
  Math::Vector3 startScale = Math::Vector3::One();
  Math::Vector3 endScale = Math::Vector3::One();

  // カラー変化（発生時 〜 消失時）
  Math::Vector4 startColor = {1.0f, 1.0f, 1.0f, 1.0f};
  Math::Vector4 endColor = {1.0f, 1.0f, 1.0f, 0.0f};

  // キルフィールド設定
  KillFieldParam killFieldParam{};

  // プールの最大粒子数
  size_t maxCount = 32;

  std::string objDirectoryPath = "./Assets/Objects/unitSphere";
  std::string objFileName = "unitSphere.obj";
  std::string mtlDirectoryPath = "./Assets/Objects/unitSphere";
  std::string mtlFileName = "unitSphere.mtl";
};

void to_json(nlohmann::json &j, const Param &param);
void from_json(const nlohmann::json &j, Param &param);

} // namespace Atrum::Visual::Particle