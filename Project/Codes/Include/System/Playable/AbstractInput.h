#pragma once

#include "Math/Vector2.h"

namespace Atrum {
class FrameDeltaTime;
}

namespace Atrum::Player {

struct InputData {
  Math::Vector2 vec2Input{};

  bool isJumpTrigger = false;
  bool isJumpPress = false;
  bool isJumpRelease = false;

  bool isAttackTrigger = false;
  bool isAttackPress = false;
  bool isAttackRelease = false;

  bool isDodgeTrigger = false;
  bool isDashPress = false;
};

/// プレイ用入力の集約（シングルトン）。Entity の Upd コンポーネントではない。
class AbstractInput final {
private:
  InputData inputData_{};
  FrameDeltaTime *frameDeltaTime_ = nullptr;

  float dodgeBufferTimer_ = 0.0f;
  float dodgeBufferTime_ = 0.25f;

  float minThresholdVec2InputLengthSq_ = 0.25f;
  float maxThresholdVec2InputLengthSq_ = 0.64f;
  float thresholdDotSameDirection_ = 0.9f;

  bool isWaitingVec2InputRelease_ = false;

  float dashStopTimer_ = 0.0f;
  float dashStopTime_ = 0.25f;
  bool isDashPress_ = false;
  bool isJumpKeyPrePush_ = false;

  Math::Vector2 vec2InputBuffer_{};

  AbstractInput() = default;
  ~AbstractInput() = default;

public:
  void ResolveDependence();
  void Update();
  const InputData &GetData() const { return inputData_; }

  static AbstractInput *Instance() {
    static AbstractInput instance;
    return &instance;
  }

  AbstractInput(const AbstractInput &) = delete;
  AbstractInput &operator=(const AbstractInput &) = delete;
};

} // namespace Atrum::Player