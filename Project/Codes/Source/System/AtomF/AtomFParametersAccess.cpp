#include "System/AtomF/AtomFParametersAccess.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersRuntime.h"

namespace Atrum::AtomF {

float ResolvePlayerBodyRadiusMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().hitRadiusMeters;
  }
  return kPlayerBodyRadiusMeters;
}

float ResolvePlayerAimCapsuleRadiusMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().aimCapsule.radiusMeters;
  }
  return kPlayerBodyRadiusMeters;
}

float ResolvePlayerBulletRadiusMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().bullet.hitRadiusMeters;
  }
  return kPlayerBulletBodyRadiusMeters;
}

float ResolvePlayerBulletSpeedMetersPerSecond() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().bullet.speedMetersPerSecond;
  }
  return kPlayerBulletSpeedMetersPerSecond;
}

float ResolvePlayerBulletTrailSeconds() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().bullet.trailSeconds;
  }
  return kPlayerBulletTrailSeconds;
}

bool IsPlayerBulletHitShapeSphere() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().bullet.hitShape == "sphere";
  }
  return true;
}

float ResolveEnemyBodyRadiusMeters() {
  // 敵本体の当たり半径は JSON にまだ無い
  (void)ParametersRuntime::Instance();
  return kEnemyBodyRadiusMeters;
}

float ResolveEnemyKnockbackSpeedMetersPerSecond() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().knockbackSpeedMetersPerSecond;
  }
  return kEnemyKnockbackSpeedMetersPerSecond;
}

float ResolveEnemyMoveSpeedMetersPerSecond() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().moveSpeedMetersPerSecond;
  }
  return kEnemyMoveSpeedMetersPerSecond;
}

} // namespace Atrum::AtomF