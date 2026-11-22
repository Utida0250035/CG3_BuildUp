#include "System/Entity/Components/CmpBallAxisAlignedBounce.h"

#include "System/Entity/Components/CmpBallBounceImpactFx.h"
#include "System/Entity/Components/CmpBallKinematics.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

void CmpBallAxisAlignedBounce::Update() {
  CmpBallKinematics *kinematics = RefOwner().GetUpdCmp<CmpBallKinematics>();
  if (kinematics == nullptr) {
    return;
  }

  CmpBallBounceImpactFx *impactFx =
      RefOwner().GetUpdCmp<CmpBallBounceImpactFx>();

  Math::Vector3 &position = RefOwner().RefTransform().translate;
  Math::Vector3 &velocity = kinematics->RefVelocity();

  const float floorLimit = groundY_ + radius_;
  if (position.y <= floorLimit && velocity.y < 0.0f) {
    position.y = floorLimit;
    velocity.y = -velocity.y * floorRestitution_;
    if (impactFx != nullptr) {
      impactFx->TrySpawnImpactAt({position.x, floorLimit, position.z},
                                 velocity.y, true);
    }
  }

  const float leftLimit = leftWallX_ + radius_;
  const float rightLimit = rightWallX_ - radius_;
  if (position.x <= leftLimit && velocity.x < 0.0f) {
    position.x = leftLimit;
    velocity.x = std::abs(velocity.x) * wallRestitution_;
    if (impactFx != nullptr) {
      impactFx->TrySpawnImpactAt({leftWallX_, position.y, position.z}, 0.0f,
                                 false);
    }
  } else if (position.x >= rightLimit && velocity.x > 0.0f) {
    position.x = rightLimit;
    velocity.x = -std::abs(velocity.x) * wallRestitution_;
    if (impactFx != nullptr) {
      impactFx->TrySpawnImpactAt({rightWallX_, position.y, position.z}, 0.0f,
                                 false);
    }
  }
}

} // namespace Atrum