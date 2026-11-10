#include "System/Entity/Components/CmpMotionBlend.h"

#include "Motion/MotionBlender.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

void CmpMotionBlend::SetClipPaths(const std::string &clipAPath,
                                  const std::string &clipBPath) {
  clipAPath_ = clipAPath;
  clipBPath_ = clipBPath;
  clipsLoaded_ = false;
}

void CmpMotionBlend::EnsureClipsLoaded() {
  if (clipsLoaded_) {
    return;
  }

  clipsLoaded_ = Motion::LoadMotionClipFromJson(clipAPath_, clipA_) &&
                 Motion::LoadMotionClipFromJson(clipBPath_, clipB_);
}

void CmpMotionBlend::Update() {
  EnsureClipsLoaded();
  if (!clipsLoaded_) {
    return;
  }

  const float delta = ::Atrum::EngineFrameDeltaTime()->GetDeltaTime();
  playbackTime_ += delta * playbackSpeed_;

  const float duration =
      std::max(clipA_.durationSeconds, clipB_.durationSeconds);
  if (duration <= 0.0f) {
    return;
  }

  if (playbackTime_ > duration) {
    if (isLooping_) {
      playbackTime_ = std::fmod(playbackTime_, duration);
    } else {
      playbackTime_ = duration;
    }
  }

  const Math::TransformLH poseA =
      Motion::SampleMotionClip(clipA_, playbackTime_);
  const Math::TransformLH poseB =
      Motion::SampleMotionClip(clipB_, playbackTime_);
  const Math::TransformLH blended =
      Motion::BlendMotionTransforms(poseA, poseB, blendWeight_);

  RefOwner().RefTransform() = blended;
}

} // namespace Atrum
