#include "System/Entity/Components/CmpBallKinematics.h"

#include "Engine/EngineAccess.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpBallKinematics::Initialize() {}

void CmpBallKinematics::Update() {
  FrameDeltaTime *frameDelta = EngineFrameDeltaTime();
  const float deltaSeconds =
      frameDelta != nullptr ? frameDelta->GetDeltaTime() : 0.0f;

  velocity_.y += gravity_ * deltaSeconds;
  RefOwner().RefTransform().translate += velocity_ * deltaSeconds;
}

} // namespace Atrum