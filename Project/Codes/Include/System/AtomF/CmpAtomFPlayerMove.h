#pragma once

#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 自機の XZ 移動（速さa）。parameters.json の moveSpeed を参照。
class CmpAtomFPlayerMove final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
};

} // namespace Atrum