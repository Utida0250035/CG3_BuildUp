#include "System/Entity/Components/CmpRotator.h"
#include "Math/Quaternion.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpRotator::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpRotator::Update() {
  if (!entPivot_) {
    return;
  }

  const Math::Quaternion deltaQ =
      Math::Quaternion::FromAxisAngle(rotateAxis_, rotateSpeed_ * frameDeltaTime_->GetDeltaTime());
  Math::Quaternion &quaternion = entPivot_->RefTransform().quaternion;
  quaternion = deltaQ * quaternion;
}

} // namespace Atrum
