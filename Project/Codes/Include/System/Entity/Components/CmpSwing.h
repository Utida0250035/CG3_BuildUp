#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class FrameDeltaTime;
class Entity;

class CmpSwing : public ComponentUpd {
private:
  float swingSpeed_ = 0.0f;
  float swingDuration_ = 0.0f;
  float swingTimer_ = 0.0f;
  Math::Vector3 swingAxis_ = Math::Vector3::RightLh();
  Entity *pivot_ = nullptr;
  FrameDeltaTime *frameDeltaTime_ = nullptr;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::MOVEMENT; }

  void Update() override;
  void ResolveDependence() override;
  void StartSwing();

  void SetPivot(Entity *pivot) { pivot_ = pivot; }
  void SetSwingDuration(const float duration) { swingDuration_ = duration; }
  void SetSwingSpeed(const float swingSpeed) { swingSpeed_ = swingSpeed; }
};

} // namespace Atrum
