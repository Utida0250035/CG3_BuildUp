#include "System/AtomF/AtomFParametersAccess.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersRuntime.h"

namespace Atrum::AtomF {

float ResolveArenaInnerRadiusMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Arena().innerRadiusMeters;
  }
  return kArenaInnerRadiusMeters;
}

float ResolveArenaWallThicknessMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Arena().wallThicknessMeters;
  }
  return kArenaWallThicknessMeters;
}

float ResolveArenaWallHeightMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Arena().wallHeightMeters;
  }
  return kArenaWallHeightMeters;
}

int ResolveArenaMeshSegmentCount() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Arena().meshSegmentCount;
  }
  return kArenaMeshSegmentCount;
}

float ResolveArenaGroundHeightMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Arena().groundHeightMeters;
  }
  return kArenaGroundHeightMeters;
}

float ResolvePlayerBodyRadiusMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().hitRadiusMeters;
  }
  return kPlayerBodyRadiusMeters;
}

float ResolvePlayerDamageVignetteSeconds() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().damageFeedback.vignetteSeconds;
  }
  return kPlayerDamageVignetteSeconds;
}

int ResolvePlayerDamageSePlayCount() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().damageFeedback.sePlayCount;
  }
  return 1;
}

int ResolvePlayerInitialHealthCount() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().initialHealthCount;
  }
  return 99;
}

float ResolvePlayerAimCapsuleRadiusMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().aimCapsule.radiusMeters;
  }
  return kPlayerAimCapsuleRadiusMeters;
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
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().hitRadiusMeters;
  }
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

float ResolveEnemyChargeIntervalSeconds() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().chargeIntervalSeconds;
  }
  return kEnemyChargeIntervalSeconds;
}

float ResolveEnemyExplosionHitRadiusMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().explosion.hitRadiusMeters;
  }
  return kEnemyExplosionHitRadiusMeters;
}

bool IsEnemyExplosionHitShapeSphere() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().explosion.hitShape == "sphere";
  }
  return true;
}

const EnemyPanicParameters &ResolveEnemyPanicParameters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().panic;
  }
  static const EnemyPanicParameters kFallback{};
  return kFallback;
}

const EnemyPeriodicChargeTimingParameters &
ResolveEnemyPeriodicChargeTimingParameters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().periodicCharge;
  }
  static const EnemyPeriodicChargeTimingParameters kFallback{};
  return kFallback;
}

const EnemyChainExplosionParameters &ResolveEnemyChainExplosionParameters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Enemy().chainExplosion;
  }
  static const EnemyChainExplosionParameters kFallback{};
  return kFallback;
}

CombatCameraParameters ResolveCombatCameraParameters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Camera();
  }
  return CombatCameraParameters{};
}

} // namespace Atrum::AtomF