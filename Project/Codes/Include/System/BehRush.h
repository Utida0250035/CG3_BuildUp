#pragma once

#include "Math/Vector3.h"
#include "System/Entity/Behavior.h"

namespace Atrum {

class FrameDeltaTime;
class CmpEnemy;

class BehRush : public Behavior {
private:
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;
  Math::Vector3 sourcePos_{};
  Math::Vector3 destinationPos_{};
  float endTime_ = 3.0f;
  float timer_ = 0.0f;

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;

  void SetDestinationPos(const Math::Vector3 &destination) { destinationPos_ = destination; }
};

} // namespace Atrum
