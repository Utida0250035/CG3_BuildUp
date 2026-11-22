#include "Physics/PhysicsManager.h"

#include "System/StageBounds.h"

namespace Atrum::Physics {

PhysicsManager &PhysicsManager::Instance() {
  static PhysicsManager instance;
  return instance;
}

void PhysicsManager::StepKinematic(Math::Vector3 &worldTranslate,
                                   Math::Vector3 &velocity,
                                   const KinematicStepSettings &settings,
                                   const float deltaTimeSeconds) {
  if (settings.useGravity) {
    velocity += settings.gravity * deltaTimeSeconds;
  }

  worldTranslate += velocity * deltaTimeSeconds;

  if (!settings.clampToStageBounds) {
    return;
  }

  if (worldTranslate.y <= kGroundHeight) {
    velocity.y = 0.0f;
    worldTranslate.y = kGroundHeight;
  } else if (worldTranslate.y > kCeilHeight) {
    velocity.y = 0.0f;
    worldTranslate.y = kCeilHeight;
  }

  if (worldTranslate.x <= kLeftEndX) {
    worldTranslate.x = kLeftEndX;
  } else if (worldTranslate.x >= kRightEndX) {
    worldTranslate.x = kRightEndX;
  }
}

} // namespace Atrum::Physics