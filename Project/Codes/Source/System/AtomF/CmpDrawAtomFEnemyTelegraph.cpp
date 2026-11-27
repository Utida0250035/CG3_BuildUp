#include "System/AtomF/CmpDrawAtomFEnemyTelegraph.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/EngineAccess.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyShooter.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <cmath>

namespace Atrum {

namespace {

/// 地面すれすれ（床 Y=0 にめり込まない高さ）に帯を描く。
constexpr float kTelegraphHeightMeters = 0.04f;

} // namespace

void CmpDrawAtomFEnemyTelegraph::ResolveDependence() {
  charge_ = RefOwner().HasUpdCmp<CmpAtomFEnemyPeriodicCharge>()
                ? RefOwner().GetUpdCmp<CmpAtomFEnemyPeriodicCharge>()
                : nullptr;
  shooter_ = RefOwner().HasUpdCmp<CmpAtomFEnemyShooter>()
                 ? RefOwner().GetUpdCmp<CmpAtomFEnemyShooter>()
                 : nullptr;
}

void CmpDrawAtomFEnemyTelegraph::Draw() {
  AtrumEngine *engine = EngineInstance();
  if (engine == nullptr) {
    return;
  }

  Math::Vector3 start = RefOwner().GetWorldPosition();
  start.y = kTelegraphHeightMeters;

  Math::Vector3 direction{0.0f, 0.0f, 0.0f};
  float length = 0.0f;
  float width = 0.1f;
  Math::Vector4 color{1.0f, 0.3f, 0.2f, 0.4f};

  if (charge_ != nullptr &&
      charge_->CurrentPhase() == CmpAtomFEnemyPeriodicCharge::Phase::Shaking) {
    // 突進の予告: 自機へ向かう進路。
    Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
    if (player == nullptr || !player->IsActive()) {
      return;
    }
    direction = player->GetWorldPosition() - RefOwner().GetWorldPosition();
    direction.y = 0.0f;
    if (direction.LengthSquare() < 1e-6f) {
      return;
    }
    direction.Normalize();
    length = AtomF::ResolveEnemyKnockbackSpeedMetersPerSecond() *
             AtomF::ResolveEnemyPeriodicChargeTimingParameters()
                 .chargeDurationSeconds;
    width = AtomF::ResolveEnemyBodyRadiusMeters() * 1.6f;
    color = Math::Vector4{1.0f, 0.35f, 0.2f, 0.28f};
  } else if (shooter_ != nullptr && shooter_->BlocksLocomotion()) {
    // 射撃の予告: 狙いが固まるほど濃く・太くなる照準線。
    const float progress = shooter_->AimProgress();
    direction = shooter_->AimDirection();
    length = 7.0f;
    width = 0.04f + 0.1f * progress;
    color = Math::Vector4{1.0f, 0.55f + 0.3f * progress, 0.2f,
                          0.2f + 0.5f * progress};
  } else {
    return;
  }

  const Math::Vector3 end = start + direction * length;
  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{}, start, end,
                     width);
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum
