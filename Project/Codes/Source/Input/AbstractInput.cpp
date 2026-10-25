#include "Input/AbstractInput.h"

#include "Input/PlayInput.h"
#include "Input/XInputController.h"
#include "System/DeltaTime.h"

#include <cmath>

namespace {

namespace I = Atrum::Input;

void ApplyKeyboardInput(I::PlayInput *input, Atrum::InputData &data) {
  if (input->IsKeyTrigger(I::Key::SPACE)) {
    data.isSubmitTrigger = true;
  }

  if (input->IsKeyTrigger(I::Key::ESCAPE)) {
    data.isPauseTrigger = true;
  }

  if (input->IsKeyTrigger(I::Key::R)) {
    data.isResetTrigger = true;
  }

  if (input->IsKeyTrigger(I::Key::SPACE)) {
    data.isJumpTrigger = true;
  }

  if (input->IsKeyTrigger(I::Key::Z)) {
    data.isZoomTrigger = true;
  }

  if (input->IsKeyPress(I::Key::A) || input->IsKeyPress(I::Key::LEFT)) {
    data.vector.x -= 1.0f;
  }

  if (input->IsKeyPress(I::Key::D) || input->IsKeyPress(I::Key::RIGHT)) {
    data.vector.x += 1.0f;
  }

  if (input->IsKeyPress(I::Key::S) || input->IsKeyPress(I::Key::DOWN)) {
    data.vector.y -= 1.0f;
  }

  if (input->IsKeyPress(I::Key::W) || input->IsKeyPress(I::Key::UP)) {
    data.vector.y += 1.0f;
  }

  if (data.vector.LengthSquare() > 1.01f) {
    data.vector.Normalize();
  }
}

void ApplyGamepadInput(I::XInputController *gamepad, Atrum::InputData &data) {
  if (!gamepad->IsConnected()) {
    return;
  }

  if (gamepad->IsButtonTrigger(I::GamepadButton::A)) {
    data.isSubmitTrigger = true;
  }

  if (gamepad->IsButtonTrigger(I::GamepadButton::Start)) {
    data.isPauseTrigger = true;
  }

  if (gamepad->IsButtonTrigger(I::GamepadButton::Back)) {
    data.isResetTrigger = true;
  }

  if (gamepad->IsButtonTrigger(I::GamepadButton::B)) {
    data.isJumpTrigger = true;
  }

  if (gamepad->IsButtonTrigger(I::GamepadButton::X)) {
    data.isZoomTrigger = true;
  }

  gamepad->AddDirectionInput(data.vector);
}

} // namespace

namespace Atrum {

void AbstractInput::Initialize() {
  input_ = I::PlayInput::Instance();
  gamepad_ = I::XInputController::Instance();
}

void AbstractInput::Update() {
  data_ = {};
  leftStickInput_ = {};

  if (gamepad_ != nullptr) {
    gamepad_->Update();
    isKeyboardInput_ = !gamepad_->IsConnected();
  } else {
    isKeyboardInput_ = true;
  }

  ApplyKeyboardInput(input_, data_);
  if (gamepad_ != nullptr) {
    ApplyGamepadInput(gamepad_, data_);

    const Math::Vector2 stick = gamepad_->GetLeftStick();
    leftStickInput_.x = static_cast<int>(std::lround(stick.x * 32767.0f));
    leftStickInput_.y = static_cast<int>(std::lround(stick.y * 32767.0f));
  }

  if (spacedVectorTimer_ <= 0.0f) {
    if (data_.vector.LengthSquare() > 1e-6f) {
      spacedVectorTimer_ = spacedVectorTime_;
      data_.timeSpacedVector = data_.vector;
    }
  }

  if (spacedVectorTimer_ > 0.0f) {
    spacedVectorTimer_ -= FrameDeltaTime::Instance()->GetDeltaTime();

    if (spacedVectorTimer_ <= 0.0f) {
      spacedVectorTimer_ = 0.0f;
    }
  }
}

} // namespace Atrum
