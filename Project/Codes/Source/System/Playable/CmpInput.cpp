#include "System/Playable/CmpInput.h"
#include "System/DeltaTime.h"

namespace Atrum::Player {

void CmpInput::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpInput::Update() {
  using Input::Key;
  Input::PlayInput *input = Input::PlayInput::Instance();

  isJumpKeyPrePush_ = inputData_.isJumpPress;
  inputData_ = {};

  if (input->IsKeyPress(Key::A)) {
    inputData_.vec2Input.x -= 1.0f;
  }
  if (input->IsKeyPress(Key::D)) {
    inputData_.vec2Input.x += 1.0f;
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
  inputData_.isJumpTrigger = input->IsKeyTrigger(Key::W);
  inputData_.isJumpPress = input->IsKeyPress(Key::W);
  inputData_.isJumpRelease = isJumpKeyPrePush_ && !inputData_.isJumpPress;
  inputData_.isAttackTrigger = input->IsKeyTrigger(Key::SPACE);
  inputData_.isAttackPress = input->IsKeyPress(Key::SPACE);
  inputData_.isAttackRelease = false;
}

} // namespace Atrum::Player
