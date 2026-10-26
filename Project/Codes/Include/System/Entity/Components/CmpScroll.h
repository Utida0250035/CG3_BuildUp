#pragma once

#include "System/DeltaTime.h"
#include "System/Entity/ComponentUpd.h"
#include "System/Entity/Entity.h"

namespace Atrum {

class CmpScroll : public ComponentUpd {
private:
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  float scrollSpeed_ = 0.0f;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::ANIMATION; }

  void ResolveDependence() override { frameDeltaTime_ = FrameDeltaTime::Instance(); }

  void Update() override {
    Math::Vector3 &translate = RefOwner().RefTransform().translate;
    translate.x += scrollSpeed_ * frameDeltaTime_->GetDeltaTime();

    constexpr float sideEndX = 48.0f;
    if (translate.x <= -sideEndX) {
      translate.x = sideEndX;
    } else if (translate.x >= sideEndX) {
      translate.x = -sideEndX;
    }
  }

  void SetScrollSpeed(const float speed) { scrollSpeed_ = speed; }
};

} // namespace Atrum
