#include "System/AtomF/CmpAtomFPlayerMove.h"

#include "Math/Vector2.h"

#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/AtomF/AtomFPlayerMoveInput.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpAtomFPlayerMove::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFPlayerMove::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  float speedMetersPerSecond = 0.5f;
  AtomF::ParametersRuntime &params = AtomF::ParametersRuntime::Instance();
  if (params.IsLoaded()) {
    speedMetersPerSecond = params.Player().moveSpeedMetersPerSecond;
  }

  const Math::Vector2 input = AtomF::SampleAtomFMoveInputXZ();
  if (input.LengthSquare() <= 0.0f) {
    return;
  }

  const float deltaSeconds = frameDeltaTime_->GetDeltaTime();
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate.x += input.x * speedMetersPerSecond * deltaSeconds;
  translate.z += input.y * speedMetersPerSecond * deltaSeconds;
}

} // namespace Atrum