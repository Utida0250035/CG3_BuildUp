#include "Motion/MotionBlender.h"

#include "Math/Lerp.h"
#include "Math/Quaternion.h"

#include <algorithm>

namespace Atrum::Motion {

namespace {

Math::TransformLH LerpTransformLh(const Math::TransformLH &from,
                                  const Math::TransformLH &to,
                                  const float t) {
  Math::TransformLH result{};
  result.translate = Math::Lerp(from.translate, to.translate, t);
  result.scale = Math::Lerp(from.scale, to.scale, t);
  result.quaternion = Math::Quaternion::Slerp(from.quaternion, to.quaternion, t);
  return result;
}

} // namespace

Math::TransformLH SampleMotionClip(const MotionClip &clip,
                                   float timeSeconds) {
  if (!clip.IsValid()) {
    return {};
  }

  const float duration = clip.durationSeconds;
  float t = std::clamp(timeSeconds, 0.0f, duration);
  if (clip.keyframes.size() == 1) {
    return clip.keyframes.front().transform;
  }

  if (t >= clip.keyframes.back().timeSeconds) {
    return clip.keyframes.back().transform;
  }

  size_t nextIndex = 1;
  while (nextIndex < clip.keyframes.size() &&
         clip.keyframes[nextIndex].timeSeconds <= t) {
    ++nextIndex;
  }

  const size_t prevIndex = nextIndex - 1;
  const MotionKeyframe &fromKf = clip.keyframes[prevIndex];
  const MotionKeyframe &toKf = clip.keyframes[nextIndex];

  const float segment =
      std::max(toKf.timeSeconds - fromKf.timeSeconds, 1.0e-4f);
  const float segmentT = std::clamp((t - fromKf.timeSeconds) / segment, 0.0f,
                                    1.0f);

  return LerpTransformLh(fromKf.transform, toKf.transform, segmentT);
}

Math::TransformLH BlendMotionTransforms(const Math::TransformLH &from,
                                        const Math::TransformLH &to,
                                        const float blendWeight01) {
  const float w = std::clamp(blendWeight01, 0.0f, 1.0f);
  return LerpTransformLh(from, to, w);
}

} // namespace Atrum::Motion
