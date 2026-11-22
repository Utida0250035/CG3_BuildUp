#include "System/AtomF/AtomFPlayerMoveInput.h"

#include "Engine/EngineAccess.h"
#include "Input/PlayInput.h"
#include "Input/XInputController.h"

namespace Atrum::AtomF {

Math::Vector2 SampleAtomFMoveInputXZ() {
  using Input::Key;
  Input::PlayInput *input = ::Atrum::EnginePlayInput();
  Math::Vector2 move{};

  if (input != nullptr) {
    if (input->IsKeyPress(Key::A)) {
      move.x -= 1.0f;
    }
    if (input->IsKeyPress(Key::D)) {
      move.x += 1.0f;
    }
    if (input->IsKeyPress(Key::W)) {
      move.y += 1.0f;
    }
    if (input->IsKeyPress(Key::S)) {
      move.y -= 1.0f;
    }
  }

  Input::XInputController *gamepad = Input::XInputController::Instance();
  if (gamepad != nullptr) {
    const Math::Vector2 stick = gamepad->GetLeftStick();
    // スティックは x→ワールド X、y→ワールド Z
    move.x += stick.x;
    move.y += stick.y;
  }

  if (move.LengthSquare() > 1.01f) {
    move.Normalize();
  }
  return move;
}

} // namespace Atrum::AtomF