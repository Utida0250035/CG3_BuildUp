#include "System/Playable/CmpInput.h"

#include "Input/XInputController.h"
#include "System/DeltaTime.h"

namespace Atrum::Player {

void CmpInput::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpInput::Update() {
  using Input::GamepadButton;
  using Input::Key;
  Input::PlayInput *input = Input::PlayInput::Instance();
  Input::XInputController *gamepad = Input::XInputController::Instance();

  isJumpKeyPrePush_ = inputData_.isJumpPress;
  inputData_ = {};

  if (input->IsKeyPress(Key::A)) {
    inputData_.vec2Input.x -= 1.0f;
  }
  if (input->IsKeyPress(Key::D)) {
    inputData_.vec2Input.x += 1.0f;
  }

  if (gamepad != nullptr) {
    gamepad->AddDirectionInput(inputData_.vec2Input);
  }

  if (inputData_.vec2Input.LengthSquare() > 1.01f) {
    inputData_.vec2Input.Normalize();
  }

  if (dodgeBufferTimer_ > 0.0f) {
    dodgeBufferTimer_ -= frameDeltaTime_->GetDeltaTime();
    if (dodgeBufferTimer_ <= 0.0f) {
      vec2InputBuffer_ = {};
      dodgeBufferTimer_ = 0.0f;
      isWaitingVec2InputRelease_ = false;
    }
  }

  if (inputData_.vec2Input.LengthSquare() >= maxThresholdVec2InputLengthSq_) {
    dashStopTimer_ = dashStopTime_;
    if (dodgeBufferTimer_ > 0.0f) {
      if (!isWaitingVec2InputRelease_ &&
          vec2InputBuffer_.Dot(inputData_.vec2Input) > thresholdDotSameDirection_) {
        inputData_.isDodgeTrigger = true;
        dodgeBufferTimer_ = 0.0f;
        isWaitingVec2InputRelease_ = false;
        isDashPress_ = true;
      }
    } else {
      vec2InputBuffer_ = inputData_.vec2Input;
      isWaitingVec2InputRelease_ = true;
      dodgeBufferTimer_ = dodgeBufferTime_;
    }
  } else if (inputData_.vec2Input.LengthSquare() <= minThresholdVec2InputLengthSq_) {
    isWaitingVec2InputRelease_ = false;
    dashStopTimer_ -= frameDeltaTime_->GetDeltaTime();
    if (dashStopTimer_ <= 0.0f) {
      dashStopTimer_ = 0.0f;
      isDashPress_ = false;
    }
  }

  inputData_.isDashPress = isDashPress_;

  const bool jumpKeyboardTrigger = input->IsKeyTrigger(Key::W);
  const bool jumpKeyboardPress = input->IsKeyPress(Key::W);
  const bool jumpPadTrigger =
      gamepad != nullptr && gamepad->IsButtonTrigger(GamepadButton::B);
  const bool jumpPadPress =
      gamepad != nullptr && gamepad->IsButtonPress(GamepadButton::B);
  inputData_.isJumpTrigger = jumpKeyboardTrigger || jumpPadTrigger;
  inputData_.isJumpPress = jumpKeyboardPress || jumpPadPress;
  inputData_.isJumpRelease = isJumpKeyPrePush_ && !inputData_.isJumpPress;

  const bool attackKeyboardTrigger = input->IsKeyTrigger(Key::SPACE);
  const bool attackKeyboardPress = input->IsKeyPress(Key::SPACE);
  const bool attackPadTrigger =
      gamepad != nullptr && gamepad->IsButtonTrigger(GamepadButton::A);
  const bool attackPadPress =
      gamepad != nullptr && gamepad->IsButtonPress(GamepadButton::A);
  inputData_.isAttackTrigger = attackKeyboardTrigger || attackPadTrigger;
  inputData_.isAttackPress = attackKeyboardPress || attackPadPress;
  inputData_.isAttackRelease = false;
}

} // namespace Atrum::Player
