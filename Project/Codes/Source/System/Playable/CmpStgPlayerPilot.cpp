#include "System/Playable/CmpStgPlayerPilot.h"

#include "Engine/EngineAccess.h"
#include "Input/PlayInput.h"
#include "Input/XInputController.h"
#include "Math/Quaternion.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpDirection.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "System/Playable/StgPilotPrimaryCameraContext.h"
#include "System/Playable/StgScreenAimOnPlane.h"

#if defined(USE_IMGUI)
#include "ForDebug/ImGui.h"
#endif

namespace Atrum::Player {

namespace {

Math::Vector2 PollKeyboardMove(Input::PlayInput *input) {
  Math::Vector2 move{};
  if (input == nullptr) {
    return move;
  }
  using Input::Key;
  if (input->IsKeyPress(Key::A)) {
    move.x -= 1.0f;
  }
  if (input->IsKeyPress(Key::D)) {
    move.x += 1.0f;
  }
  if (input->IsKeyPress(Key::S)) {
    move.y -= 1.0f;
  }
  if (input->IsKeyPress(Key::W)) {
    move.y += 1.0f;
  }
  if (move.LengthSquare() > 1.01f) {
    move.Normalize();
  }
  return move;
}

Math::Vector3 ToGameplayAimDir(const Math::Vector2 &v) {
  if (v.LengthSquare() <= 1e-6f) {
    return {};
  }
  Math::Vector3 dir{v.x, v.y, 0.0f};
  return dir.Normalized();
}

} // namespace

void CmpStgPlayerPilot::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  cmpLhRigidBody_ = RefOwner().GetUpdCmp<CmpLhRigidBody>();
  cmpDirection_ = RefOwner().GetDataCmp<CmpDirection>();
  cmpChildEntitys_ = RefOwner().GetUpdCmp<CmpChildEntitys>();
}

void CmpStgPlayerPilot::Initialize() {
  if (cmpLhRigidBody_ != nullptr) {
    cmpLhRigidBody_->SetIsUseGravity(false);
    cmpLhRigidBody_->SetIsUseSpaceEnd(false);
  }
  if (!RefOwner().HasUpdCmp<CmpChildEntitys>()) {
    RefOwner().AddUpdCmp<CmpChildEntitys>();
    cmpChildEntitys_ = RefOwner().GetUpdCmp<CmpChildEntitys>();
  }
}

void CmpStgPlayerPilot::Update() {
  using Input::GamepadButton;
  using Input::Key;

  if (frameDeltaTime_ == nullptr || cmpLhRigidBody_ == nullptr) {
    return;
  }

  const float deltaTime = frameDeltaTime_->GetDeltaTime();
  if (shootCooldownTimer_ > 0.0f) {
    shootCooldownTimer_ -= deltaTime;
    if (shootCooldownTimer_ < 0.0f) {
      shootCooldownTimer_ = 0.0f;
    }
  }

  Input::PlayInput *input = ::Atrum::EnginePlayInput();
  Input::XInputController *gamepad = Input::XInputController::Instance();

  Math::Vector2 move = PollKeyboardMove(input);
  if (gamepad != nullptr && gamepad->IsConnected()) {
    const Math::Vector2 stick = gamepad->GetLeftStick();
    move.x += stick.x;
    move.y += stick.y;
    if (move.LengthSquare() > 1.01f) {
      move.Normalize();
    }
  }

  Math::Vector3 &velocity = cmpLhRigidBody_->RefVelocity();
  velocity.x = move.x * moveSpeed_;
  velocity.y = move.y * moveSpeed_;
  velocity.z = 0.0f;

  Math::Vector3 newAim{};
  bool aimUpdated = false;

  if (gamepad != nullptr && gamepad->IsConnected()) {
    const Math::Vector2 rightStick = gamepad->GetRightStick();
    newAim = ToGameplayAimDir(rightStick);
    if (newAim.LengthSquare() > 1e-6f) {
      aimUpdated = true;
    }
  }

#if defined(USE_IMGUI)
  const bool imguiWantsMouse = ImGui::GetIO().WantCaptureMouse;
#else
  const bool imguiWantsMouse = false;
#endif

  if (!aimUpdated && input != nullptr && !imguiWantsMouse) {
    const Camera *camera = StgPilotPrimaryCameraContext::PrimaryCamera();
    int32_t viewW = 0;
    int32_t viewH = 0;
    if (camera != nullptr &&
        StgPilotPrimaryCameraContext::ViewportSize(viewW, viewH)) {
      const Math::Vector2 cursor = input->GetCursorPos();
      Math::Vector3 hit{};
      const float planeY = RefOwner().GetWorldPosition().y;
      if (TryScreenPointToHorizontalPlaneY(*camera, cursor.x, cursor.y,
                                           static_cast<float>(viewW),
                                           static_cast<float>(viewH), planeY,
                                           hit)) {
        Math::Vector3 toTarget = hit - RefOwner().GetWorldPosition();
        toTarget.z = 0.0f;
        if (toTarget.LengthSquare() > 1e-6f) {
          newAim = toTarget.Normalized();
          aimUpdated = true;
        }
      }
    }
  }

  if (aimUpdated) {
    aimDirection_ = newAim;
  }

  if (cmpDirection_ != nullptr) {
    cmpDirection_->SetDirection(aimDirection_);
  }

  if (aimDirection_.LengthSquare() > 1e-6f) {
    RefOwner().RefTransform().quaternion = Math::Quaternion::FromLhLookAt(
        aimDirection_, Math::Vector3::UpLh());
  }

  const bool shootTrigger =
      (input != nullptr && input->IsKeyTrigger(Key::SPACE)) ||
      (gamepad != nullptr && gamepad->IsButtonTrigger(GamepadButton::A));

  if (shootTrigger && shootCooldownTimer_ <= 0.0f &&
      aimDirection_.LengthSquare() > 1e-6f && cmpChildEntitys_ != nullptr) {
    const Math::Vector3 spawnPos =
        RefOwner().GetWorldPosition() + aimDirection_ * bulletSpawnOffset_;
    std::unique_ptr<Entity> bullet = EntityFactory::LinearPlayerBullet(
        aimDirection_ * bulletSpeed_, spawnPos);
    cmpChildEntitys_->AddChilds({bullet.release()});
    shootCooldownTimer_ = shootCooldownSeconds_;
  }
}

} // namespace Atrum::Player