#include "System/AtomF/CmpDrawAtomFBoss.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

namespace {

constexpr float kTwoPi = 6.28318530718f;
constexpr float kFloorY = 0.04f;

void DrawFloorRing(AtrumEngine &engine, const Math::Vector3 &center,
                   const float radius, const float width,
                   const Math::Vector4 &color) {
  constexpr int kSegments = 48;
  for (int i = 0; i < kSegments; ++i) {
    const float a0 = kTwoPi * static_cast<float>(i) / kSegments;
    const float a1 = kTwoPi * static_cast<float>(i + 1) / kSegments;
    const Math::Vector3 p0{center.x + std::cos(a0) * radius, kFloorY,
                           center.z + std::sin(a0) * radius};
    const Math::Vector3 p1{center.x + std::cos(a1) * radius, kFloorY,
                           center.z + std::sin(a1) * radius};
    engine.DrawLine3DOnFloor(WhiteTextureIndex(), color, Math::Transform{}, p0,
                             p1, width);
  }
}

/// 原点中心の円（アリーナ）と、start から dir 方向のレイが交わるまでの距離。
float RayLengthToArena(const Math::Vector3 &start, const Math::Vector3 &dir,
                       const float innerRadius) {
  const float b = start.x * dir.x + start.z * dir.z;
  const float c = start.x * start.x + start.z * start.z - innerRadius * innerRadius;
  const float disc = b * b - c;
  if (disc <= 0.0f) {
    return 0.0f;
  }
  return std::max(0.0f, -b + std::sqrt(disc));
}

} // namespace

void CmpDrawAtomFBoss::ResolveDependence() {
  boss_ = RefOwner().GetUpdCmp<CmpAtomFBoss>();
}

void CmpDrawAtomFBoss::Draw() {
  if (boss_ == nullptr) {
    return;
  }
  AtrumEngine *engine = EngineInstance();
  if (engine == nullptr) {
    return;
  }

  const AtomF::BossParameters &params = AtomF::ResolveBossParameters();
  const Math::Vector3 pos = RefOwner().GetWorldPosition();
  const Math::Vector3 floorPos{pos.x, kFloorY, pos.z};
  const float p = boss_->StateProgress();
  const float pulse = 0.5f + 0.5f * std::sin(p * 40.0f);

  switch (boss_->GetState()) {
  case CmpAtomFBoss::State::TelegraphCharge: {
    // 突進の進路（ボスの直径幅）。進むほど濃くなる。
    const Math::Vector3 dir = boss_->ChargeDirection();
    const float length = RayLengthToArena(
        floorPos, dir, AtomF::ResolveArenaInnerRadiusMeters());
    const Math::Vector4 color{1.0f, 0.15f, 0.1f, 0.12f + 0.3f * p * (0.6f + 0.4f * pulse)};
    engine->DrawLine3DOnFloor(WhiteTextureIndex(), color, Math::Transform{},
                              floorPos, floorPos + dir * length,
                              params.bodyRadiusMeters * 2.0f);
    break;
  }
  case CmpAtomFBoss::State::TelegraphSummon: {
    const Math::Vector4 color{0.2f, 0.9f, 1.0f, 0.2f + 0.5f * p};
    for (int i = 0; i < params.summonCount; ++i) {
      const Math::Vector3 spawn = boss_->SummonSpawnPosition(i);
      DrawFloorRing(*engine, spawn, 0.2f + 0.35f * (1.0f - p), 0.08f, color);
    }
    break;
  }
  case CmpAtomFBoss::State::TelegraphNova: {
    // 外周は範囲、内側の輪が溜まるにつれて広がる。
    DrawFloorRing(*engine, floorPos, params.novaRadiusMeters, 0.12f,
                  Math::Vector4{1.0f, 0.55f, 0.1f, 0.35f + 0.4f * pulse});
    DrawFloorRing(*engine, floorPos, params.novaRadiusMeters * p, 0.18f,
                  Math::Vector4{1.0f, 0.8f, 0.3f, 0.3f + 0.4f * p});
    break;
  }
  default:
    break;
  }

  // 頭上の Hp バー（出現後〜撃破前のみ）。
  if (boss_->IsTargetable()) {
    const float ratio = boss_->HealthRatio();
    const float halfWidth = std::max(1.2f, params.bodyRadiusMeters * 1.4f);
    const float y = pos.y + params.bodyRadiusMeters + 0.9f;
    const Math::Vector3 left{pos.x - halfWidth, y, pos.z};
    const Math::Vector3 right{pos.x + halfWidth, y, pos.z};
    engine->DrawLine3D(WhiteTextureIndex(), Math::Vector4{0.05f, 0.05f, 0.08f, 0.75f},
                       Math::Transform{}, left, right, 0.26f);
    if (ratio > 0.0f) {
      const Math::Vector3 fillEnd{left.x + (right.x - left.x) * ratio, y, pos.z};
      const Math::Vector4 fill = boss_->IsEnraged()
                                     ? Math::Vector4{1.0f, 0.25f, 0.2f, 0.95f}
                                     : Math::Vector4{0.95f, 0.85f, 0.25f, 0.95f};
      engine->DrawLine3D(WhiteTextureIndex(), fill, Math::Transform{}, left,
                         fillEnd, 0.18f);
    }
  }
}

} // namespace Atrum
