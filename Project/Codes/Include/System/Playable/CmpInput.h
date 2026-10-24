#pragma once

#include "Input/PlayInput.h"
#include "Math/Vector2.h"
#include "System/Entity/ComponentUpd.h"

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

class CmpInput : public ComponentUpd {
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

  CmpInput() = default;
  ~CmpInput() = default;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::INPUT; }

  void ResolveDependence() override;
  void Update() override;
  const InputData &GetData() const { return inputData_; }

  static CmpInput *Instance() {
    static CmpInput instance;
    return &instance;
  }

  CmpInput(const CmpInput &) = delete;
  CmpInput &operator=(const CmpInput &) = delete;
};

} // namespace Atrum::Player
