#include "System/AtomF/AtomFEnemyArenaWallContact.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum::AtomF {

bool IsEnemyTouchingArenaWall(const Entity &enemy) {
  const Math::Vector3 position = enemy.GetWorldPosition();
  return IsWorldXZTouchingArenaWall(position.x, position.z,
                                    ResolveEnemyBodyRadiusMeters());
}

} // namespace Atrum::AtomF