#include "System/AtomF/CmpAtomFEnemyWalkToPlayer.h"

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

void CmpAtomFEnemyWalkToPlayer::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFEnemyWalkToPlayer::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
    if (CmpAtomFEnemyChainExplosion *chain =
            RefOwner().GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      return;
    }
  }

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()) {
    if (CmpAtomFEnemyKnockback *knockback =
            RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>();
        knockback != nullptr && knockback->HasActiveKnockback()) {
      return;
    }
  }

  if (CmpAtomFEnemyPeriodicCharge *charge =
          RefOwner().GetUpdCmp<CmpAtomFEnemyPeriodicCharge>();
      charge != nullptr && charge->BlocksLocomotion()) {
    return;
  }

  if (CmpAtomFEnemyTargetedPanic *panic =
          RefOwner().GetUpdCmp<CmpAtomFEnemyTargetedPanic>();
      panic != nullptr && panic->BlocksLocomotion()) {
    return;
  }

  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return;
  }

  const Math::Vector3 playerPos = player->GetWorldPosition();
  const Math::Vector3 selfPos = RefOwner().GetWorldPosition();
  Math::Vector3 toPlayer = playerPos - selfPos;
  toPlayer.y = 0.0f;
  const float distSq = toPlayer.LengthSquare();
  if (distSq <= 1e-6f) {
    return;
  }

  toPlayer *= (1.0f / std::sqrt(distSq));

  const float speed = AtomF::ResolveEnemyMoveSpeedMetersPerSecond();
  const float dt = frameDeltaTime_->GetDeltaTime();
  RefOwner().RefTransform().translate += toPlayer * speed * dt;
}

} // namespace Atrum