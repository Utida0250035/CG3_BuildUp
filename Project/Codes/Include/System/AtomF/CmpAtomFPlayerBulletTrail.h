#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

#include <vector>

namespace Atrum {

struct AtomFPlayerBulletTrailSample {
  float ageSeconds = 0.0f;
  Math::Vector3 worldPosition{};
};

/// atomF 自機弾の軌道。parameters の trailSeconds 分サンプルを残す。
class CmpAtomFPlayerBulletTrail final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PRE_RENDER;
  }

  void ResolveDependence() override;
  void Update() override;
  void Finalize() override;

  void BeginLingerPhase();
  bool IsLingerPhase() const { return lingerPhase_; }

  const std::vector<AtomFPlayerBulletTrailSample> &Samples() const {
    return samples_;
  }
  float TrailDurationSeconds() const { return trailDurationSeconds_; }

private:
  void CompleteLingerDestroyIfReady();

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  std::vector<AtomFPlayerBulletTrailSample> samples_{};
  float trailDurationSeconds_ = 0.5f;
  bool lingerPhase_ = false;
  bool destroyRequested_ = false;
};

} // namespace Atrum