#include "System/AtomF/AtomFArenaBounds.h"

#include "System/AtomF/AtomFParametersAccess.h"

#include <cmath>

namespace Atrum::AtomF {

void ClampWorldTranslateToArenaDisc(Math::Vector3 &worldTranslate,
                                    const float bodyRadiusMeters) {
  const float maxRadius = ResolveArenaInnerRadiusMeters() - bodyRadiusMeters;
  if (maxRadius <= 0.0f) {
    worldTranslate.x = 0.0f;
    worldTranslate.z = 0.0f;
    return;
  }

  const float x = worldTranslate.x;
  const float z = worldTranslate.z;
  const float distSq = x * x + z * z;
  const float maxSq = maxRadius * maxRadius;
  if (distSq <= maxSq) {
    return;
  }

  const float dist = std::sqrt(distSq);
  if (dist <= 0.0f) {
    return;
  }

  const float scale = maxRadius / dist;
  worldTranslate.x = x * scale;
  worldTranslate.z = z * scale;
}

bool IsWorldXZTouchingArenaWall(const float x, const float z,
                                const float bodyRadiusMeters) {
  const float maxRadius = ResolveArenaInnerRadiusMeters() - bodyRadiusMeters;
  if (maxRadius <= 0.0f) {
    return false;
  }

  const float distSq = x * x + z * z;
  const float threshold = maxRadius * maxRadius * 0.96f;
  return distSq >= threshold;
}

} // namespace Atrum::AtomF