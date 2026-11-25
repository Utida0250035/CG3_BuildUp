#include "System/AtomF/CmpAtomFPlayerFire.h"

#include "Math/Vector3.h"

#include "Engine/EngineAccess.h"
#include "Input/PlayInput.h"
#include "Input/XInputController.h"
#include "System/AtomF/AtomFPlayerBulletSpawn.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/CmpAtomFPlayerAim.h"
#include "System/AtomF/CmpAtomFPlayerShotCooldown.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpAtomFPlayerFire::ResolveDependence() {
  aim_ = RefOwner().GetUpdCmp<CmpAtomFPlayerAim>();
  shotCooldown_ = RefOwner().GetUpdCmp<CmpAtomFPlayerShotCooldown>();
}

void CmpAtomFPlayerFire::Update() {
  if (aim_ == nullptr || shotCooldown_ == nullptr) {
    return;
  }

  using Input::GamepadButton;
  using Input::Key;
  Input::PlayInput *input = ::Atrum::EnginePlayInput();
  Input::XInputController *gamepad = Input::XInputController::Instance();

  const bool keyboardFire =
      input != nullptr && input->IsKeyTrigger(Key::SPACE);
  const bool padFire =
      gamepad != nullptr && gamepad->IsButtonTrigger(GamepadButton::A);
  if (!keyboardFire && !padFire) {
    return;
  }

  if (!shotCooldown_->TryConsumeShotCooldown()) {
    return;
  }

  const Math::Vector3 direction = aim_->GetAimDirectionXZ();
  const Math::Vector3 spawnWorld = RefOwner().GetWorldPosition();

  std::unique_ptr<Entity> bullet =
      AtomF::CreatePlayerBulletEntity(direction, spawnWorld);
  if (bullet == nullptr) {
    return;
  }

  AtomF::EnqueueSceneEntityAdopt(std::move(bullet));
}

} // namespace Atrum
