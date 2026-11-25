#include "System/AtomF/CmpAtomFPlayerBulletPredictCapsule.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFPlayerAimCapsuleRayClamp.h"
#include "System/AtomF/CmpAtomFPlayerBulletMove.h"
#include "System/Entity/Entity.h"

namespace Atrum {

namespace {

float MaxPredictCapsuleLengthMeters() {
  return AtomF::ResolveArenaInnerRadiusMeters() * 2.0f;
}

} // namespace

void CmpAtomFPlayerBulletPredictCapsule::ResolveDependence() {
  move_ = RefOwner().GetUpdCmp<CmpAtomFPlayerBulletMove>();
  radiusMeters_ = AtomF::ResolvePlayerBulletRadiusMeters();
}

Math::Vector3 CmpAtomFPlayerBulletPredictCapsule::CapsuleStartWorld() {
  return RefOwner().GetWorldPosition();
}

Math::Vector3 CmpAtomFPlayerBulletPredictCapsule::DirectionXZUnit() const {
  if (move_ == nullptr) {
    return Math::Vector3{0.0f, 0.0f, 1.0f};
  }
  return move_->GetDirectionXZUnit();
}

Math::Vector3 CmpAtomFPlayerBulletPredictCapsule::CapsuleEndWorld() {
  const Math::Vector3 start = CapsuleStartWorld();
  if (currentLengthMeters_ <= 0.0f) {
    return start;
  }
  return start + DirectionXZUnit() * currentLengthMeters_;
}

void CmpAtomFPlayerBulletPredictCapsule::Update() {
  if (move_ == nullptr) {
    currentLengthMeters_ = 0.0f;
    return;
  }

  radiusMeters_ = AtomF::ResolvePlayerBulletRadiusMeters();
  const Math::Vector3 start = CapsuleStartWorld();
  const Math::Vector3 dir = DirectionXZUnit();
  const float maxLength = MaxPredictCapsuleLengthMeters();
  // 弾自身を除外し、壁と敵で長さをクランプする。
  currentLengthMeters_ = AtomF::ResolveAimCapsuleClampedMaxLength(
      RefOwner(), start, dir, radiusMeters_, maxLength);
}

} // namespace Atrum
