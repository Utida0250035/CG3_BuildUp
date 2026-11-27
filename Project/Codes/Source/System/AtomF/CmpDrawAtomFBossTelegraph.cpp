#include "System/AtomF/CmpDrawAtomFBossTelegraph.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/EngineAccess.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr float kGroundY = 0.05f;
constexpr float kTwoPi = 6.28318530718f;
constexpr int kCircleSegments = 40;

void DrawGroundCircle(AtrumEngine *engine, const Math::Vector3 &center,
                      const float radius, const Math::Vector4 &color,
                      const float width) {
  if (radius <= 0.01f) {
    return;
  }
  for (int i = 0; i < kCircleSegments; ++i) {
    const float a0 =
        kTwoPi * static_cast<float>(i) / static_cast<float>(kCircleSegments);
    const float a1 = kTwoPi * static_cast<float>(i + 1) /
                     static_cast<float>(kCircleSegments);
    const Math::Vector3 p0{center.x + std::cos(a0) * radius, kGroundY,
                           center.z + std::sin(a0) * radius};
    const Math::Vector3 p1{center.x + std::cos(a1) * radius, kGroundY,
                           center.z + std::sin(a1) * radius};
    engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{}, p0, p1,
                       width);
  }
}

} // namespace

void CmpDrawAtomFBossTelegraph::ResolveDependence() {
  boss_ = RefOwner().HasUpdCmp<CmpAtomFBoss>()
              ? RefOwner().GetUpdCmp<CmpAtomFBoss>()
              : nullptr;
}

void CmpDrawAtomFBossTelegraph::Draw() {
  if (boss_ == nullptr) {
    return;
  }
  AtrumEngine *engine = EngineInstance();
  if (engine == nullptr) {
    return;
  }

  switch (boss_->CurrentState()) {
  case CmpAtomFBoss::State::Appear:
  case CmpAtomFBoss::State::Idle:
  case CmpAtomFBoss::State::RingFire:
  case CmpAtomFBoss::State::SlamRecover:
  case CmpAtomFBoss::State::Dashing:
  case CmpAtomFBoss::State::Stunned:
  case CmpAtomFBoss::State::Dying:
    return; // 予告を出さない状態では何も描かず、ブレンド状態にも触れない。
  default:
    break;
  }

  const float progress = boss_->TelegraphProgress();
  const float pulse = 0.5f + 0.5f * std::sin(progress * 40.0f);
  Math::Vector3 bossPos = RefOwner().GetWorldPosition();
  bossPos.y = kGroundY;

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  switch (boss_->CurrentState()) {
  case CmpAtomFBoss::State::RingTelegraph: {
    // 弾が飛ぶ放射線。溜まるほど濃くなる。
    const int count = boss_->CurrentRingBulletCount();
    const Math::Vector4 color{1.0f, 0.3f, 0.85f, 0.15f + 0.4f * progress};
    for (int i = 0; i < count; ++i) {
      const float angle =
          kTwoPi * static_cast<float>(i) / static_cast<float>(count);
      const Math::Vector3 end{bossPos.x + std::cos(angle) * 6.0f, kGroundY,
                              bossPos.z + std::sin(angle) * 6.0f};
      engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{}, bossPos,
                         end, 0.05f);
    }
    break;
  }
  case CmpAtomFBoss::State::SummonTelegraph: {
    // 子分の出現位置。膨らんで確定する。
    const Math::Vector4 color{0.8f, 0.5f, 1.0f, 0.3f + 0.4f * pulse};
    for (const Math::Vector3 &p : boss_->SummonPoints()) {
      DrawGroundCircle(engine, p, 0.15f + 0.45f * progress, color, 0.07f);
    }
    break;
  }
  case CmpAtomFBoss::State::SlamTelegraph:
  case CmpAtomFBoss::State::SlamLeap: {
    // 叩きつけの予告円。外周は範囲、内側の円が満ちて着弾までの時間を示す。
    const bool leaping = boss_->CurrentState() == CmpAtomFBoss::State::SlamLeap;
    const float fill = leaping ? 1.0f : progress;
    const Math::Vector3 &target = boss_->SlamTarget();
    const float radius = boss_->SlamRadiusMeters();
    DrawGroundCircle(engine, target, radius,
                     Math::Vector4{1.0f, 0.25f, 0.15f, 0.55f}, 0.12f);
    DrawGroundCircle(engine, target, radius * fill,
                     Math::Vector4{1.0f, 0.45f, 0.2f, 0.3f + 0.3f * pulse},
                     0.35f);
    break;
  }
  case CmpAtomFBoss::State::DashTelegraph: {
    // 突進の進路。直前は帯が濃くなる（最初は追尾、途中で固定）。
    const Math::Vector3 end = bossPos + boss_->DashDirection() * 40.0f;
    const float bodyWidth = AtomF::ResolveBossParameters().bodyRadiusMeters * 1.8f;
    engine->DrawLine3D(WhiteTextureIndex(),
                       Math::Vector4{1.0f, 0.2f, 0.2f, 0.12f + 0.3f * progress},
                       Math::Transform{}, bossPos, end, bodyWidth);
    break;
  }
  case CmpAtomFBoss::State::Enrage: {
    // 怒り: 外へ広がる波紋。
    DrawGroundCircle(engine, bossPos, 1.5f + 8.0f * progress,
                     Math::Vector4{1.0f, 0.3f, 0.2f, 0.7f * (1.0f - progress)},
                     0.2f);
    break;
  }
  default:
    break;
  }
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum
