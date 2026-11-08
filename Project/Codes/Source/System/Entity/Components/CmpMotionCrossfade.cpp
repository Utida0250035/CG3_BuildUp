#include "System/Entity/Components/CmpMotionCrossfade.h"

#include "Motion/MotionClip.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

bool CmpMotionCrossfade::LoadClipFromPath(
    const std::string &jsonPath, Motion::MotionClip &outClip) const {
  return Motion::LoadMotionClipFromJson(jsonPath, outClip);
}

bool CmpMotionCrossfade::PlayClip(const std::string &jsonPath) {
  Motion::MotionClip clip{};
  if (!LoadClipFromPath(jsonPath, clip)) {
    return false;
  }

  crossfade_.SetPrimaryClip(std::move(clip));
  return true;
}

bool CmpMotionCrossfade::CrossfadeToClip(const std::string &jsonPath,
                                         const float durationSeconds) {
  Motion::MotionClip clip{};
  if (!LoadClipFromPath(jsonPath, clip)) {
    return false;
  }

  crossfade_.BeginCrossfadeTo(std::move(clip), durationSeconds);
  return true;
}

void CmpMotionCrossfade::Update() {
  const float delta = FrameDeltaTime::Instance()->GetDeltaTime();
  crossfade_.Advance(delta);
  RefOwner().RefTransform() = crossfade_.EvaluatePose();
}

} // namespace Atrum
