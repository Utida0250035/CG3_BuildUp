#include "System/AtomF/CmpAtomFPlayerFire.h"

#include "Math/Vector3.h"

#include "Engine/EngineAccess.h"
#include "Input/PlayInput.h"
#include "Input/XInputController.h"
#include "System/AtomF/AtomFPlayerBulletSpawn.h"
#include "System/AtomF/CmpAtomFPlayerAim.h"
#include "System/AtomF/CmpAtomFPlayerShotCooldown.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpAtomFPlayerFire::ResolveDependence() {
  aim_ = RefOwner().GetUpdCmp<CmpAtomFPlayerAim>();
  shotCooldown_ = RefOwner().GetUpdCmp<CmpAtomFPlayerShotCooldown>();
  childHolder_ = RefOwner().GetUpdCmp<CmpChildEntitys>();
}

void CmpAtomFPlayerFire::Update() {
  if (aim_ == nullptr || shotCooldown_ == nullptr || childHolder_ == nullptr) {
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
  const Math::Vector3 spawnPos = RefOwner().GetWorldPosition();

  std::unique_ptr<Entity> bullet =
      AtomF::CreatePlayerBulletEntity(spawnPos, direction);
  if (bullet == nullptr) {
    return;
  }

  Entity *bulletRaw = bullet.release();
  childHolder_->AddChilds({bulletRaw});
}

} // namespace Atrum