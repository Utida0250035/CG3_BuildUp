#include "System/AtomF/CmpAtomFPlayerAim.h"

#include "Camera/ActiveCameraSystem.h"
#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Input/PlayInput.h"
#include "Input/XInputController.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "System/Editor/EditorEditViewScreenRay.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr float kRightStickAimDeadZoneSq = 0.04f;

float DirectionToAngleRadians(const float dirX, const float dirZ) {
  return std::atan2(dirX, dirZ);
}

bool TryAimAngleFromMouse(const Entity &owner, float &outAngleRadians) {
  Input::PlayInput *input = ::Atrum::EnginePlayInput();
  AtrumEngine *engine = AtrumEngine::Instance();
  Camera *camera = ActiveCameraSystem::Instance().GetActiveCamera();
  if (input == nullptr || engine == nullptr || camera == nullptr) {
    return false;
  }

  int32_t viewWidth = 0;
  int32_t viewHeight = 0;
  if (!engine->TryGetClientViewportSize(viewWidth, viewHeight)) {
    return false;
  }

  const Math::Vector2 cursor = input->GetCursorPos();
  const Math::Vector3 origin = owner.GetWorldPosition();
  Math::Vector3 hit{};
  if (!Editor::TryPickHorizontalPlaneY(*camera, cursor.x, cursor.y,
                                     static_cast<float>(viewWidth),
                                     static_cast<float>(viewHeight),
                                     origin.y, hit)) {
    return false;
  }

  const float dx = hit.x - origin.x;
  const float dz = hit.z - origin.z;
  if (dx * dx + dz * dz < 1e-6f) {
    return false;
  }

  outAngleRadians = DirectionToAngleRadians(dx, dz);
  return true;
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
  float newAngle = aimAngleRadians_;

  if (TryAimAngleFromRightStick(newAngle)) {
    aimAngleRadians_ = newAngle;
    ApplyYawToOwner(RefOwner(), aimAngleRadians_);
    return;
  }

  if (TryAimAngleFromMouse(RefOwner(), newAngle)) {
    aimAngleRadians_ = newAngle;
    ApplyYawToOwner(RefOwner(), aimAngleRadians_);
  }
}

} // namespace Atrum