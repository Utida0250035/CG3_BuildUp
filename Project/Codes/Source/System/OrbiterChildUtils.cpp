#include "System/OrbiterChildUtils.h"

#include "System/CmpChildEntitys.h"
#include "System/CmpOrbiter.h"
#include "System/Entity/Entity.h"

#include <cmath>
#include <memory>
#include <numbers>

namespace Atrum::OrbiterChildUtils {

size_t CountBossOrbiters(Entity &boss) {
  if (!boss.HasUpdCmp<CmpChildEntitys>()) {
    return 0;
  }

  size_t count = 0;
  CmpChildEntitys *childEntitys = boss.GetUpdCmp<CmpChildEntitys>();
  for (const std::unique_ptr<Entity> &child : childEntitys->RefChilds()) {
    if (child && child->HasUpdCmp<CmpOrbiter>()) {
      ++count;
    }
  }
  return count;
}

void ForEachBossOrbiter(
    Entity &boss,
    const std::function<void(CmpOrbiter &, size_t index, size_t orbiterCount)>
        &callback) {
  if (!boss.HasUpdCmp<CmpChildEntitys>()) {
    return;
  }

  const size_t orbiterCount = CountBossOrbiters(boss);
  if (orbiterCount == 0) {
    return;
  }

  CmpChildEntitys *childEntitys = boss.GetUpdCmp<CmpChildEntitys>();
  size_t index = 0;
  for (const std::unique_ptr<Entity> &child : childEntitys->RefChilds()) {
    if (!child || !child->HasUpdCmp<CmpOrbiter>()) {
      continue;
    }
    callback(*child->GetUpdCmp<CmpOrbiter>(), index, orbiterCount);
    ++index;
  }
}

void ForEachBossOrbiter(
    Entity &boss, const std::function<void(CmpOrbiter &)> &callback) {
  ForEachBossOrbiter(boss, [&](CmpOrbiter &orbiter, size_t, size_t) {
    callback(orbiter);
  });
}

void UpdateBossOrbiterMovementOnly(Entity &boss) {
  if (!boss.HasUpdCmp<CmpChildEntitys>()) {
    return;
  }

  CmpChildEntitys *childEntitys = boss.GetUpdCmp<CmpChildEntitys>();
  for (const std::unique_ptr<Entity> &child : childEntitys->RefChilds()) {
    if (!child || !child->HasUpdCmp<CmpOrbiter>()) {
      continue;
    }
    child->GetUpdCmp<CmpOrbiter>()->Update();
  }
}

void ClampBossOrbiterDistanceFromBoss(Entity &boss, const float maxDistance) {
  if (!boss.HasUpdCmp<CmpChildEntitys>() || maxDistance <= 0.0f) {
    return;
  }

  CmpChildEntitys *childEntitys = boss.GetUpdCmp<CmpChildEntitys>();
  for (const std::unique_ptr<Entity> &child : childEntitys->RefChilds()) {
    if (!child || !child->HasUpdCmp<CmpOrbiter>()) {
      continue;
    }

    Math::Vector3 &localTranslate = child->RefTransform().translate;
    const float lengthSq = localTranslate.LengthSquare();
    const float maxSq = maxDistance * maxDistance;
    if (lengthSq <= maxSq || lengthSq <= 1e-8f) {
      continue;
    }

    localTranslate =
        localTranslate * (maxDistance / std::sqrt(lengthSq));
  }
}

void ForEachBossOrbiterEntity(
    Entity &boss,
    const std::function<void(Entity &orbiterEntity, size_t index,
                             size_t orbiterCount)> &callback) {
  if (!boss.HasUpdCmp<CmpChildEntitys>()) {
    return;
  }

  const size_t orbiterCount = CountBossOrbiters(boss);
  if (orbiterCount == 0) {
    return;
  }

  CmpChildEntitys *childEntitys = boss.GetUpdCmp<CmpChildEntitys>();
  size_t index = 0;
  for (const std::unique_ptr<Entity> &child : childEntitys->RefChilds()) {
    if (!child || !child->HasUpdCmp<CmpOrbiter>()) {
      continue;
    }
    callback(*child, index, orbiterCount);
    ++index;
  }
}

float OrbiterIndexToStartAngleRadians(const size_t index,
                                      const size_t orbiterCount) {
  if (orbiterCount == 0) {
    return 0.0f;
  }

  return (2.0f * std::numbers::pi_v<float> * static_cast<float>(index)) /
         static_cast<float>(orbiterCount);
}

} // namespace Atrum::OrbiterChildUtils
