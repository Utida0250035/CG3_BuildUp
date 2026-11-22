#include "System/AtomF/AtomFEnemyArenaWallContact.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum::AtomF {

bool IsEnemyTouchingArenaWall(const Entity &enemy) {
  const float bodyRadius = ResolveEnemyBodyRadiusMeters();
  const float maxRadius = kArenaInnerRadiusMeters - bodyRadius;
  if (maxRadius <= 0.0f) {
    return false;
  }

  const Math::Vector3 position = enemy.GetWorldPosition();
  const float distSq = position.x * position.x + position.z * position.z;
  const float threshold = maxRadius * maxRadius * 0.96f;
  return distSq >= threshold;
}

} // namespace Atrum::AtomF