#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"
#include <vector>

namespace Atrum {

struct HitSphere {
  Math::Vector3 localOffset{};
  float radius;
};

class Entity;
class HitJudge;

class CmpHitSphere : public ComponentUpd {
private:
  HitJudge *hitJudge_ = nullptr;

  std::vector<HitSphere> hitSpheres_{};
  std::vector<Entity *> hitEntitys_{};
  bool isActive_ = true;

public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_JUDGE;
  }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;
  void Finalize() override;

  std::vector<HitSphere> GetSpheres() const { return hitSpheres_; }
  void SetSpheres(const std::vector<HitSphere> &hitSpheres) {
    hitSpheres_ = hitSpheres;
  }

  void SetIsActive(const bool isActive) { isActive_ = isActive; }
  bool GetIsActive() const { return isActive_; }

  std::vector<Entity *> GetHitEntitys();
  
  ~CmpHitSphere() {
      Finalize();
  }

};

} // namespace Atrum