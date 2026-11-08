#pragma once

#include "Motion/MotionClip.h"
#include "Math/Transform.h"

namespace Atrum::Motion {

/// クリップ A から B への時間付きクロスフェード
class MotionCrossfade {
public:
  void SetPrimaryClip(MotionClip clip);
  void BeginCrossfadeTo(MotionClip nextClip, float durationSeconds);

  void Advance(float deltaSeconds);

  bool IsCrossfading() const { return isCrossfading_; }

  Math::TransformLH EvaluatePose() const;

  float GetPrimaryPlaybackTime() const { return primaryTime_; }
  float GetSecondaryPlaybackTime() const { return secondaryTime_; }

private:
  MotionClip primaryClip_{};
  MotionClip secondaryClip_{};
  bool hasPrimary_ = false;
  bool hasSecondary_ = false;

  float primaryTime_ = 0.0f;
  float secondaryTime_ = 0.0f;
  float crossfadeDuration_ = 0.0f;
  float crossfadeElapsed_ = 0.0f;
  bool isCrossfading_ = false;

  void CommitSecondaryAsPrimary();
};

} // namespace Atrum::Motion
