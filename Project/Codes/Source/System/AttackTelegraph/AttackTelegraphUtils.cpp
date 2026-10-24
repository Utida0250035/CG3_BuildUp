#include "System/AttackTelegraph/AttackTelegraphUtils.h"

#include "System/CmpAttackTelegraph.h"
#include "System/CmpChildEntitys.h"
#include "System/CmpOrbiter.h"
#include "System/Entity/Entity.h"
#include "System/StageBounds.h"

#include <algorithm>
#include <vector>
#include <cmath>
#include <limits>

namespace Atrum {

Math::Vector3 ExtendRayWithinStageBounds(
    const Math::Vector3 &start, const Math::Vector3 &directionNormalized,
    const float maxLength) {
  constexpr float kBoundaryEpsilon = 0.05f;
  float tMax = maxLength;

  if (std::abs(directionNormalized.x) > 1.0e-5f) {
    const float boundX =
        directionNormalized.x > 0.0f ? kRightEndX : kLeftEndX;
    const float t = (boundX - start.x) / directionNormalized.x;
    if (t > kBoundaryEpsilon) {
      tMax = std::min(tMax, t);
    }
  }

  if (std::abs(directionNormalized.y) > 1.0e-5f) {
    const float boundY =
        directionNormalized.y > 0.0f ? kCeilHeight : kGroundHeight;
    const float t = (boundY - start.y) / directionNormalized.y;
    if (t > kBoundaryEpsilon) {
      tMax = std::min(tMax, t);
    }
  }

  return start + directionNormalized * tMax;
}

void AddVerticalDropTelegraph(CmpAttackTelegraph *telegraph, const float worldX,
                              const float spawnY, const float groundY) {
  if (telegraph == nullptr) {
    return;
  }

  AttackTelegraphSegment segment{};
  segment.start = {worldX, spawnY, 0.0f};
  segment.end = {worldX, groundY, 0.0f};
  telegraph->AddSegment(segment);
}

void RegisterWallShotDropTelegraph(CmpAttackTelegraph *telegraph,
                                   const float spawnY, const float groundY,
                                   const uint32_t shotCount) {
  if (telegraph == nullptr) {
    return;
  }

  for (float x = 0.5f + kLeftEndX; x < kRightEndX; x += 6.0f) {
    AddVerticalDropTelegraph(telegraph, x, spawnY, groundY);
  }

  std::vector<float> slowBulletShotX;
  for (float x = 0.5f + kLeftEndX; x < kRightEndX; x += 8.0f) {
    slowBulletShotX.push_back(x);
  }
  if (shotCount * 2ULL < slowBulletShotX.size()) {
    slowBulletShotX.erase(slowBulletShotX.begin() +
                          static_cast<std::ptrdiff_t>(shotCount * 2ULL));
  }
  for (const float shotX : slowBulletShotX) {
    AddVerticalDropTelegraph(telegraph, shotX, spawnY, groundY);
  }
}

void RegisterFullWidthGroundLines(CmpAttackTelegraph *telegraph,
                                  const float groundY,
                                  const float secondaryLineYOffset,
                                  const Math::Vector4 &mainColor,
                                  const Math::Vector4 &secondaryColor) {
  if (telegraph == nullptr) {
    return;
  }

  telegraph->AddLine({kLeftEndX, groundY, 0.0f}, {kRightEndX, groundY, 0.0f},
                     mainColor);
  telegraph->AddLine({kLeftEndX, groundY + secondaryLineYOffset, 0.0f},
                     {kRightEndX, groundY + secondaryLineYOffset, 0.0f},
                     secondaryColor);
}

void RegisterBossOrbiterGatherTelegraph(Entity &boss,
                                        CmpAttackTelegraph *telegraph,
                                        const Math::Vector4 &lineColor) {
  if (telegraph == nullptr || !boss.HasUpdCmp<CmpChildEntitys>()) {
    return;
  }

  CmpChildEntitys *childEntitys = boss.GetUpdCmp<CmpChildEntitys>();
  for (const std::unique_ptr<Entity> &child : childEntitys->RefChilds()) {
    if (!child || !child->HasUpdCmp<CmpOrbiter>()) {
      continue;
    }

    CmpOrbiter *orbiter = child->GetUpdCmp<CmpOrbiter>();
    telegraph->AddLine(child->GetWorldPosition(),
                       orbiter->GetGatherTargetWorldPosition(), lineColor);
  }
}

} // namespace Atrum
