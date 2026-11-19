#include "System/Entity/Components/CmpBounceBall.h"

#include "System/Entity/Components/CmpBallAxisAlignedBounce.h"
#include "System/Entity/Components/CmpBallBounceImpactFx.h"
#include "System/Entity/Components/CmpBallKinematics.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpBounceBall::Initialize() { EnsureAndSyncChildComponents(); }

void CmpBounceBall::EnsureAndSyncChildComponents() {
  Entity &entity = RefOwner();

  if (!entity.HasUpdCmp<CmpBallKinematics>()) {
    entity.AddUpdCmp<CmpBallKinematics>();
  }
  if (!entity.HasUpdCmp<CmpBallAxisAlignedBounce>()) {
    entity.AddUpdCmp<CmpBallAxisAlignedBounce>();
  }
  if (!entity.HasUpdCmp<CmpBallBounceImpactFx>()) {
    entity.AddUpdCmp<CmpBallBounceImpactFx>();
  }

  CmpBallKinematics *kinematics = entity.GetUpdCmp<CmpBallKinematics>();
  CmpBallAxisAlignedBounce *bounce =
      entity.GetUpdCmp<CmpBallAxisAlignedBounce>();
  CmpBallBounceImpactFx *impactFx = entity.GetUpdCmp<CmpBallBounceImpactFx>();
  if (kinematics == nullptr || bounce == nullptr || impactFx == nullptr) {
    return;
  }

  kinematics->SetInitialVelocity(velocity_);
  kinematics->SetGravity(gravity_);
  bounce->SetLeftWallX(leftWallX_);
  bounce->SetRightWallX(rightWallX_);
  bounce->SetGroundY(groundY_);
  bounce->SetRadius(radius_);
  bounce->SetWallRestitution(wallRestitution_);
  bounce->SetFloorRestitution(floorRestitution_);
  impactFx->SetImpactJsonPath(impactJsonPath_);
  impactFx->SetImpactCooldownSeconds(impactCooldownSeconds_);
}

} // namespace Atrum