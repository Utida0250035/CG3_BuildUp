#pragma once

#include "Motion/MotionCrossfade.h"
#include "System/Entity/ComponentUpd.h"

#include <string>

namespace Atrum {

/// 単一クリップ再生と、別クリップへの補間遷移
class CmpMotionCrossfade final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void Update() override;

  bool PlayClip(const std::string &jsonPath);
  bool CrossfadeToClip(const std::string &jsonPath, float durationSeconds);

  bool IsCrossfading() const { return crossfade_.IsCrossfading(); }

private:
  Motion::MotionCrossfade crossfade_{};

  bool LoadClipFromPath(const std::string &jsonPath,
                        Motion::MotionClip &outClip) const;
};

} // namespace Atrum
