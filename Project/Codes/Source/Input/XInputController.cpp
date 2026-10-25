#include "Input/XInputController.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <XInput.h>

#pragma comment(lib, "xinput.lib")

#include <algorithm>
#include <cmath>
#include <cstring>

namespace Atrum::Input {

struct XInputController::Private {
  XINPUT_STATE state{};
  XINPUT_STATE previousState{};
};

namespace {

constexpr DWORD kUserIndex = 0;

float ApplyCircularDeadZone(const float axis, const float deadZoneRatio) {
  const float magnitude = std::fabs(axis);
  if (magnitude <= deadZoneRatio) {
    return 0.0f;
  }

  const float scaled =
      (magnitude - deadZoneRatio) / std::max(1.0f - deadZoneRatio, 1e-6f);
  return std::copysign(scaled, axis);
}

} // namespace

XInputController::XInputController() : private_(std::make_unique<Private>()) {}

XInputController::~XInputController() = default;

XInputController *XInputController::Instance() {
  static XInputController instance;
  return &instance;
}

void XInputController::Update() {
  Private &data = *private_;
  data.previousState = data.state;
  std::memset(&data.state, 0, sizeof(XINPUT_STATE));

  const DWORD result = XInputGetState(kUserIndex, &data.state);
  isConnected_ = (result == ERROR_SUCCESS);
}

uint16_t XInputController::ButtonMask(const GamepadButton button) const {
  switch (button) {
  case GamepadButton::A:
    return XINPUT_GAMEPAD_A;
  case GamepadButton::B:
    return XINPUT_GAMEPAD_B;
  case GamepadButton::X:
    return XINPUT_GAMEPAD_X;
  case GamepadButton::Y:
    return XINPUT_GAMEPAD_Y;
  case GamepadButton::Start:
    return XINPUT_GAMEPAD_START;
  case GamepadButton::Back:
    return XINPUT_GAMEPAD_BACK;
  case GamepadButton::DpadUp:
    return XINPUT_GAMEPAD_DPAD_UP;
  case GamepadButton::DpadDown:
    return XINPUT_GAMEPAD_DPAD_DOWN;
  case GamepadButton::DpadLeft:
    return XINPUT_GAMEPAD_DPAD_LEFT;
  case GamepadButton::DpadRight:
    return XINPUT_GAMEPAD_DPAD_RIGHT;
  case GamepadButton::LeftShoulder:
    return XINPUT_GAMEPAD_LEFT_SHOULDER;
  case GamepadButton::RightShoulder:
    return XINPUT_GAMEPAD_RIGHT_SHOULDER;
  }
  return 0;
}

bool XInputController::IsButtonPress(const GamepadButton button) const {
  if (!isConnected_ || private_ == nullptr) {
    return false;
  }

  const XINPUT_STATE &state = private_->state;
  const uint16_t mask = ButtonMask(button);
  return (state.Gamepad.wButtons & mask) != 0;
}

bool XInputController::IsButtonTrigger(const GamepadButton button) const {
  if (!isConnected_ || private_ == nullptr) {
    return false;
  }

  const uint16_t mask = ButtonMask(button);
  const bool now = (private_->state.Gamepad.wButtons & mask) != 0;
  const bool prev = (private_->previousState.Gamepad.wButtons & mask) != 0;
  return now && !prev;
}

bool XInputController::IsButtonRelease(const GamepadButton button) const {
  if (!isConnected_ || private_ == nullptr) {
    return false;
  }

  const uint16_t mask = ButtonMask(button);
  const bool now = (private_->state.Gamepad.wButtons & mask) != 0;
  const bool prev = (private_->previousState.Gamepad.wButtons & mask) != 0;
  return !now && prev;
}

Math::Vector2 XInputController::GetLeftStick() const {
  if (!isConnected_ || private_ == nullptr) {
    return {};
  }

  const XINPUT_GAMEPAD &gamepad = private_->state.Gamepad;
  const float rawX = static_cast<float>(gamepad.sThumbLX) / 32767.0f;
  const float rawY = static_cast<float>(gamepad.sThumbLY) / 32767.0f;

  Math::Vector2 stick{ApplyCircularDeadZone(rawX, kStickDeadZoneRatio),
                      ApplyCircularDeadZone(rawY, kStickDeadZoneRatio)};

  if (stick.LengthSquare() > 1.01f) {
    stick.Normalize();
  }

  return stick;
}

void XInputController::AddDirectionInput(Math::Vector2 &vector) const {
  if (!isConnected_) {
    return;
  }

  const Math::Vector2 stick = GetLeftStick();
  vector.x += stick.x;
  vector.y += stick.y;

  if (IsButtonPress(GamepadButton::DpadLeft)) {
    vector.x -= 1.0f;
  }
  if (IsButtonPress(GamepadButton::DpadRight)) {
    vector.x += 1.0f;
  }
  if (IsButtonPress(GamepadButton::DpadDown)) {
    vector.y -= 1.0f;
  }
  if (IsButtonPress(GamepadButton::DpadUp)) {
    vector.y += 1.0f;
  }

  if (vector.LengthSquare() > 1.01f) {
    vector.Normalize();
  }
}

} // namespace Atrum::Input
