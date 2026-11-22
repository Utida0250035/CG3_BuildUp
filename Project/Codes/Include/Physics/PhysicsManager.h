#pragma once

#include "Math/Vector3.h"

namespace Atrum::Physics {

struct KinematicStepSettings {
  Math::Vector3 gravity{};
  bool useGravity = true;
  bool clampToStageBounds = true;
};

/// 形状データを持たない運動学ステップ（重力・速度積分・ステージ境界）
class PhysicsManager {
public:
  static PhysicsManager &Instance();

  void StepKinematic(Math::Vector3 &worldTranslate, Math::Vector3 &velocity,
                     const KinematicStepSettings &settings,
                     float deltaTimeSeconds);
};

} // namespace Atrum::Physics