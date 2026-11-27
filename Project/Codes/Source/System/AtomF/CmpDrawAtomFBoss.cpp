#include "System/AtomF/CmpDrawAtomFBoss.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/EngineAccess.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr float kTwoPi = 6.28318530718f;
constexpr int kRingSegments = 40;
constexpr float kFloorY = 0.04f;

void DrawFloorCircle(AtrumEngine *engine, const Math::Vector3 &center,
                     const float radius, const float width,
                     const Math::Vector4 &color) {
  if (radius <= 1e-3f) {
    return;
  }
  for (int i = 0; i < kRingSegments; ++i) {
    const float a0 = kTwoPi * static_cast<float>(i) / kRingSegments;
    const float a1 = kTwoPi * static_cast<float>(i + 1) / kRingSegments;
    const Math::Vector3 p0{center.x + std::cos(a0) * radius, kFloorY,
                           center.z + std::sin(a0) * radius};
    const Math::Vector3 p1{center.x + std::cos(a1) * radius, kFloorY,
                           center.z + std::sin(a1) * radius};
    engine->DrawLine3DOnFloor(WhiteTextureIndex(), color, Math::Transform{}, p0,
                              p1, width);
  }
}

/// 床の円を塗りつぶす代わりに、同心円を細かく重ねて半透明の面に見せる。
void DrawFloorDisc(AtrumEngine *engine, const Math::Vector3 &center,
                   const float radius, const Math::Vector4 &color) {
  constexpr int kRings = 6;
  for (int i = 1; i <= kRings; ++i) {
    const float r = radius * static_cast<float>(i) / kRings;
    DrawFloorCircle(engine, center, r, radius / kRings * 1.1f, color);
  }
}

} // namespace

void CmpDrawAtomFBoss::ResolveDependence() {
  boss_ = RefOwner().GetUpdCmp<CmpAtomFBoss>();
}

void CmpDrawAtomFBoss::Draw() {
  if (boss_ == nullptr || !boss_->HasTelegraph()) {
    return;
  }
  AtrumEngine *engine = EngineInstance();
  if (engine == nullptr) {
    return;
  }

  const float k = boss_->TelegraphProgress01();
  const Math::Vector3 bossPos = RefOwner().GetWorldPosition();
  engine->SetBlendModeForFrame(BlendMode::NORMAL);

  switch (boss_->GetState()) {
  case CmpAtomFBoss::State::ShockwaveTelegraph: {
    // 範囲の外周は常時、内側が発動に向けて塗りつぶされるように広がる。
    const float radius = boss_->ShockwaveRadiusMeters();
    DrawFloorCircle(engine, bossPos, radius, 0.12f,
                    Math::Vector4{1.0f, 0.25f, 0.2f, 0.85f});
    DrawFloorDisc(engine, bossPos, radius * k,
                  Math::Vector4{1.0f, 0.2f, 0.15f, 0.22f});
    break;
  }
  case CmpAtomFBoss::State::DashTelegraph: {
    const Math::Vector3 dir = boss_->DashDirection();
    const float len = boss_->DashLineLengthMeters();
    const Math::Vector3 end{bossPos.x + dir.x * len, kFloorY,
                            bossPos.z + dir.z * len};
    const Math::Vector3 start{bossPos.x, kFloorY, bossPos.z};
    const float width = boss_->DashLineWidthMeters();
    engine->DrawLine3DOnFloor(WhiteTextureIndex(),
                              Math::Vector4{1.0f, 0.25f, 0.2f, 0.18f},
                              Math::Transform{}, start, end, width);
    // 進行に合わせて中心線の濃い帯が伸びる。
    const Math::Vector3 filled{bossPos.x + dir.x * len * k, kFloorY,
                               bossPos.z + dir.z * len * k};
    engine->DrawLine3DOnFloor(WhiteTextureIndex(),
                              Math::Vector4{1.0f, 0.3f, 0.2f, 0.45f},
                              Math::Transform{}, start, filled, width);
    break;
  }
  case CmpAtomFBoss::State::SummonTelegraph: {
    for (const Math::Vector3 &point : boss_->SummonPoints()) {
      DrawFloorCircle(engine, point, 0.55f, 0.1f,
                      Math::Vector4{0.75f, 0.35f, 1.0f, 0.9f});
      DrawFloorDisc(engine, point, 0.55f * k,
                    Math::Vector4{0.7f, 0.3f, 1.0f, 0.3f});
    }
    break;
  }
  default:
    break;
  }
}

} // namespace Atrum
