#include "System/BossCombatEnrage.h"

#include "System/Entity/Components/CmpOrbiter.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Entity.h"
#include "System/OrbiterChildUtils.h"

namespace Atrum::BossCombatEnrage {

namespace {

constexpr float kOrbiterSpeedScaleEnraged = 1.55f;
constexpr float kShotTempoScaleEnraged = 1.45f;

} // namespace

bool IsEnraged(Entity &boss) {
  const CmpHealthHit *health = boss.GetUpdCmp<CmpHealthHit>();
  if (health == nullptr || health->GetInitialHealthCount() == 0) {
    return false;
  }

  return health->GetHealthCount() <= health->GetInitialHealthCount() / 2;
}

float GetShotTempoScale(Entity &boss) {
  return IsEnraged(boss) ? kShotTempoScaleEnraged : 1.0f;
}

float ScaleShotCoolTimeSeconds(Entity &boss,
                               const float baseCoolTimeSeconds) {
  const float tempo = GetShotTempoScale(boss);
  if (tempo <= 0.0f) {
    return baseCoolTimeSeconds;
  }
  return baseCoolTimeSeconds / tempo;
}

uint32_t ScaleShotCountMax(Entity &boss, const uint32_t baseShotCountMax) {
  if (!IsEnraged(boss)) {
    return baseShotCountMax;
  }
  return baseShotCountMax + 1;
}

void SyncOrbiterRotateSpeeds(Entity &boss) {
  const float orbiterScale =
      IsEnraged(boss) ? kOrbiterSpeedScaleEnraged : 1.0f;

  OrbiterChildUtils::ForEachBossOrbiter(boss, [orbiterScale](CmpOrbiter &orbiter) {
    orbiter.ApplyRotateSpeedScale(orbiterScale);
  });
}

} // namespace Atrum::BossCombatEnrage
