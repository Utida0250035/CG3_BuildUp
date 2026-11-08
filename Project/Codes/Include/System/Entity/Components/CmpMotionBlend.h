#pragma once

#include "Motion/MotionClip.h"
#include "System/Entity/ComponentUpd.h"

#include <string>

namespace Atrum {

/// 2 つのモーションクリップをブレンドして Entity Transform に適用
class CmpMotionBlend final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void Update() override;

  void SetClipPaths(const std::string &clipAPath, const std::string &clipBPath);
  void SetBlendWeight(const float weight01) { blendWeight_ = weight01; }
  void SetPlaybackSpeed(const float speed) { playbackSpeed_ = speed; }
  void SetLooping(const bool loop) { isLooping_ = loop; }

  float GetBlendWeight() const { return blendWeight_; }

private:
  std::string clipAPath_{};
  std::string clipBPath_{};
  Motion::MotionClip clipA_{};
  Motion::MotionClip clipB_{};
  bool clipsLoaded_ = false;

  float playbackTime_ = 0.0f;
  float playbackSpeed_ = 1.0f;
  float blendWeight_ = 0.0f;
  bool isLooping_ = true;

  void EnsureClipsLoaded();
};

} // namespace Atrum
