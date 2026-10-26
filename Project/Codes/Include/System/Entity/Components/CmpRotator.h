#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class FrameDeltaTime;
class Entity;

class CmpRotator : public ComponentUpd {
private:
  Math::Vector3 rotateAxis_ = Math::Vector3::UpLh();
  float rotateSpeed_ = 0.0f;
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  Entity *entPivot_ = nullptr;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::MOVEMENT; }

  void ResolveDependence() override;
  void Update() override;

  void SetRotateAxis(const Math::Vector3 &axis) { rotateAxis_ = axis; }
  void SetRotateSpeed(const float rotateSpeed) { rotateSpeed_ = rotateSpeed; }
  void SetPivot(Entity *entPivot) { entPivot_ = entPivot; }
  Entity &RefPivot() { return *entPivot_; }
};

} // namespace Atrum
