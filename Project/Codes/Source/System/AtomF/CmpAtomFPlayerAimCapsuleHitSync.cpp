#include "System/AtomF/CmpAtomFPlayerAimCapsuleHitSync.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFPlayerAimCapsule.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"

#include <cmath>
#include <vector>

namespace Atrum {

void CmpAtomFPlayerAimCapsuleHitSync::ResolveDependence() {
  capsule_ = RefOwner().GetUpdCmp<CmpAtomFPlayerAimCapsule>();
  hitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
}

void CmpAtomFPlayerAimCapsuleHitSync::Update() {
  if (hitSphere_ == nullptr) {
    return;
  }

  const float bodyRadius = AtomF::ResolvePlayerBodyRadiusMeters();
  std::vector<HitSphere> spheres{{{Math::Vector3{}, bodyRadius}}};

  if (capsule_ != nullptr && capsule_->CurrentLengthMeters() > 1e-4f) {
    const float capsuleRadius = capsule_->RadiusMeters();
    const Math::Vector3 start = capsule_->CapsuleStartWorld();
    const Math::Vector3 end = capsule_->CapsuleEndWorld();
    const Math::Vector3 axis = end - start;
    const float length = axis.Length();
    if (length > 1e-4f) {
      const Math::Vector3 direction = axis * (1.0f / length);
      const float step = std::max(capsuleRadius * 0.5f, 0.05f);
      for (float distance = step; distance <= length; distance += step) {
        const Math::Vector3 worldPoint = start + direction * distance;
        const Math::Vector3 offset = worldPoint - RefOwner().GetWorldPosition();
        spheres.push_back({offset, capsuleRadius});
      }
    }
  }

  hitSphere_->SetSpheres(spheres);
  hitSphere_->SetIsActive(true);
}

} // namespace Atrum