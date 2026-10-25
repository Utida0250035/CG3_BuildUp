#pragma once

#include "Math/Vector2.h"

#include <cstdint>
#include <memory>

namespace Atrum::Input {

/// XINPUT ゲームパッド（プレイヤー 0）
enum class GamepadButton : uint8_t {
  A,
  B,
  X,
  Y,
  Start,
  Back,
  DpadUp,
  DpadDown,
  DpadLeft,
  DpadRight,
  LeftShoulder,
  RightShoulder,
};

class XInputController final {
public:
  static XInputController *Instance();

  XInputController(const XInputController &) = delete;
  XInputController &operator=(const XInputController &) = delete;

  ~XInputController();

  void Update();

  bool IsConnected() const { return isConnected_; }

  bool IsButtonPress(const GamepadButton button) const;
  bool IsButtonTrigger(const GamepadButton button) const;
  bool IsButtonRelease(const GamepadButton button) const;

  /// 左スティック（デッドゾーン適用・最大長 1）
  Math::Vector2 GetLeftStick() const;

  /// 十字キーとスティックを -1〜1 の移動ベクトルに加算
  void AddDirectionInput(Math::Vector2 &vector) const;

private:
  XInputController();

  struct Private;

  static constexpr float kStickDeadZoneRatio = 0.24f;

  std::unique_ptr<Private> private_;
  bool isConnected_ = false;

  uint16_t ButtonMask(const GamepadButton button) const;
};

} // namespace Atrum::Input
