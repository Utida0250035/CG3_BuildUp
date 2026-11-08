#include "Motion/MotionCrossfade.h"

#include "Motion/MotionBlender.h"
#include "Math/Easing.h"

#include <algorithm>

namespace Atrum::Motion {

void MotionCrossfade::SetPrimaryClip(MotionClip clip) {
  primaryClip_ = std::move(clip);
  hasPrimary_ = primaryClip_.IsValid();
  primaryTime_ = 0.0f;
  isCrossfading_ = false;
  hasSecondary_ = false;
}

void MotionCrossfade::BeginCrossfadeTo(MotionClip nextClip,
                                       const float durationSeconds) {
  if (!hasPrimary_ || !nextClip.IsValid()) {
    return;
  }

  secondaryClip_ = std::move(nextClip);
  hasSecondary_ = true;
  secondaryTime_ = 0.0f;
  crossfadeDuration_ = std::max(durationSeconds, 1.0e-4f);
  crossfadeElapsed_ = 0.0f;
  isCrossfading_ = true;
}

void MotionCrossfade::CommitSecondaryAsPrimary() {
  primaryClip_ = std::move(secondaryClip_);
  primaryTime_ = secondaryTime_;
  hasSecondary_ = false;
  isCrossfading_ = false;
  crossfadeElapsed_ = 0.0f;
}

void MotionCrossfade::Advance(float deltaSeconds) {
  if (!hasPrimary_) {
    return;
  }

  primaryTime_ += deltaSeconds;

  if (!isCrossfading_) {
    return;
  }

  crossfadeElapsed_ += deltaSeconds;
  secondaryTime_ += deltaSeconds;

  if (crossfadeElapsed_ >= crossfadeDuration_) {
    CommitSecondaryAsPrimary();
  }
}

Math::TransformLH MotionCrossfade::EvaluatePose() const {
  if (!hasPrimary_) {
    return {};
  }

  const Math::TransformLH posePrimary =
      SampleMotionClip(primaryClip_, primaryTime_);

  if (!isCrossfading_ || !hasSecondary_) {
    return posePrimary;
  }

  const float t =
      std::clamp(crossfadeElapsed_ / crossfadeDuration_, 0.0f, 1.0f);
  const float eased = Math::Ease::InOutQuad(t);
  const Math::TransformLH poseSecondary =
      SampleMotionClip(secondaryClip_, secondaryTime_);

  return BlendMotionTransforms(posePrimary, poseSecondary, eased);
}

} // namespace Atrum::Motion
