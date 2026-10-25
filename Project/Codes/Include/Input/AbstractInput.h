#pragma once

#include "Math/Vector2.h"

namespace Atrum {

namespace Input {

class PlayInput;
class XInputController;

}

// 無印: キー, b: ボタン
struct InputData {
  // 決定 Space, bA
  bool isSubmitTrigger = false;
  // ポーズ ESC, bSTART
  bool isPauseTrigger = false;
  // リセット R, bBACK
  bool isResetTrigger = false;
  // ジャンプ W, bB
  bool isJumpTrigger = false;
  // ズーム Z, bX
  bool isZoomTrigger = false;

  // 方向 W,A,S,D, スティック, 十字ボタン
  Math::Vector2 vector{};
  // 方向(間隔あり), W,A,S,D, スティック, 十字ボタン
  Math::Vector2 timeSpacedVector{};

  // 退出 キーコン未定
  bool isQuitTrigger = false;
};

struct StickInput {
  int x = 0;
  int y = 0;
};

class AbstractInput final {
private:
  InputData data_{};

  Input::PlayInput *input_ = nullptr;
  Input::XInputController *gamepad_ = nullptr;

  float spacedVectorTimer_ = 0.0f;
  float spacedVectorTime_ = 0.5f;

  StickInput leftStickInput_{};

  AbstractInput() = default;
  ~AbstractInput() = default;

  // キーボードかの判定
  bool isKeyboardInput_ = false;

public:
  void Initialize();
  void Update();

  const InputData &GetData() { return data_; }

  static AbstractInput *Instance() {
    static AbstractInput instance;
    return &instance;
  }

  AbstractInput(const AbstractInput &source) = delete;
  AbstractInput operator=(const AbstractInput &source) = delete;
};

} // namespace Atrum