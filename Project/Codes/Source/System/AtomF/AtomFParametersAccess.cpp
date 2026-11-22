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

float ResolvePlayerBulletRadiusMeters() {
  ParametersRuntime &runtime = ParametersRuntime::Instance();
  if (runtime.IsLoaded()) {
    return runtime.Player().bullet.hitRadiusMeters;
  }
  return kPlayerBulletBodyRadiusMeters;
}

float ResolveEnemyBodyRadiusMeters() {
  // 敵本体の当たり半径は JSON にまだ無い
  (void)ParametersRuntime::Instance();
  return kEnemyBodyRadiusMeters;
}

} // namespace Atrum::AtomF