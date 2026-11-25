#include "System/AtomF/CmpAtomFPlayerAim.h"

#include "Input/XInputController.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "System/AtomF/AtomFPlayerAimMousePick.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr float kRightStickAimDeadZoneSq = 0.04f;

float DirectionToAngleRadians(const float dirX, const float dirZ) {
  return std::atan2(dirX, dirZ);
}

bool TryAimAngleFromRightStick(float &outAngleRadians) {
  Input::XInputController *gamepad = Input::XInputController::Instance();
  if (gamepad == nullptr) {
    return false;
  }

  const Math::Vector2 stick = gamepad->GetRightStick();
  if (stick.LengthSquare() < kRightStickAimDeadZoneSq) {
    return false;
  }

  outAngleRadians = DirectionToAngleRadians(stick.x, stick.y);
  return true;
}

void ApplyYawToOwner(Entity &owner, const float angleRadians) {
  Math::TransformLH &transform = owner.RefTransform();
  transform.quaternion =
      Math::Quaternion::FromAxisAngle(Math::Vector3::UpLh(), angleRadians);
}

} // namespace

Math::Vector3 CmpAtomFPlayerAim::GetAimDirectionXZ() const {
  const float sinA = std::sin(aimAngleRadians_);
  const float cosA = std::cos(aimAngleRadians_);
  return {sinA, 0.0f, cosA};
}

void CmpAtomFPlayerAim::Update() {
  aimInputActive_ = false;
  float newAngle = aimAngleRadians_;

  if (TryAimAngleFromRightStick(newAngle)) {
    aimAngleRadians_ = newAngle;
    ApplyYawToOwner(RefOwner(), aimAngleRadians_);
    aimInputActive_ = true;
    return;
  }

  if (AtomF::TryResolveAimAngleFromMouse(RefOwner(), newAngle)) {
    aimAngleRadians_ = newAngle;
    ApplyYawToOwner(RefOwner(), aimAngleRadians_);
    aimInputActive_ = true;
  }
}

} // namespace Atrum