#include "System/AttackTelegraph/AttackTelegraphUtils.h"

#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "System/AttackTelegraph/CmpAttackTelegraph.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpOrbiter.h"
#include "System/Entity/Entity.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <limits>
#include <unordered_map>
#include <vector>

namespace Atrum {

namespace {

bool ColorsNearlyEqual(const Math::Vector4 &a, const Math::Vector4 &b) {
  constexpr float kEpsilon = 1.0e-3f;
  return std::abs(a.x - b.x) < kEpsilon && std::abs(a.y - b.y) < kEpsilon &&
         std::abs(a.z - b.z) < kEpsilon && std::abs(a.w - b.w) < kEpsilon;
}

/// 赤予測線表示区間ごとに警告 SE を1回だけ鳴らす
std::unordered_map<CmpAttackTelegraph *, bool> bossMeleeWarnPlayedByTelegraph{};

} // namespace

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

  const Math::Vector3 leftMain{kLeftEndX, groundY, 0.0f};
  const Math::Vector3 rightMain{kRightEndX, groundY, 0.0f};
  if (IsBossMeleeTelegraphColor(mainColor)) {
    AddBossMeleeTelegraphLine(telegraph, leftMain, rightMain);
  } else {
    telegraph->AddLine(leftMain, rightMain, mainColor);
  }

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

    Math::TransformLH gatherLocal{};
    gatherLocal.translate = child->GetInitialTransform().translate;
    const Math::Matrix4x4 gatherWorldMatrix =
        gatherLocal.MakeWorldMatrix() * boss.GetWorldMatrix();
    const Math::Vector3 gatherTargetWorld{gatherWorldMatrix[3][0],
                                          gatherWorldMatrix[3][1],
                                          gatherWorldMatrix[3][2]};
    if (IsBossMeleeTelegraphColor(lineColor)) {
      AddBossMeleeTelegraphLine(telegraph, child->GetWorldPosition(),
                                gatherTargetWorld);
    } else {
      telegraph->AddLine(child->GetWorldPosition(), gatherTargetWorld,
                         lineColor);
    }
  }
}

void PlayBulletWarningSound() {
  const std::string sePath = PlaySeDetail::ResolveExistingSePath(
      {"./Assets/Audios/seBossBulletWarn.mp3",
       "./Resources/seBossBulletWarn.mp3"});
  if (sePath.empty()) {
    return;
  }

  PlaySe(sePath);
}

void PlayBossMeleeWarnSound() {
  const std::string sePath = PlaySeDetail::ResolveExistingSePath(
      {"./Assets/Audios/seBossMeleeWarn.mp3",
       "./Resources/seBossMeleeWarn.mp3"});
  if (sePath.empty()) {
    return;
  }

  PlaySe(sePath);
}

bool IsBossMeleeTelegraphColor(const Math::Vector4 &color) {
  return ColorsNearlyEqual(color, AttackTelegraphPalette::kNonReflectAbleMain) ||
         ColorsNearlyEqual(color, AttackTelegraphPalette::kNonReflectAbleSub);
}

void BeginBossMeleeTelegraphSession(CmpAttackTelegraph *telegraph) {
  if (telegraph == nullptr) {
    return;
  }

  bossMeleeWarnPlayedByTelegraph[telegraph] = false;
}

void ResetBossMeleeTelegraphWarnSessions() {
  bossMeleeWarnPlayedByTelegraph.clear();
}

void AddBossMeleeTelegraphLine(CmpAttackTelegraph *telegraph,
                               const Math::Vector3 &start,
                               const Math::Vector3 &end) {
  if (telegraph == nullptr) {
    return;
  }

  bool &warnPlayed = bossMeleeWarnPlayedByTelegraph[telegraph];
  if (!warnPlayed) {
    PlayBossMeleeWarnSound();
    warnPlayed = true;
  }

  telegraph->AddLine(start, end, AttackTelegraphPalette::kNonReflectAbleMain);
}

} // namespace Atrum
