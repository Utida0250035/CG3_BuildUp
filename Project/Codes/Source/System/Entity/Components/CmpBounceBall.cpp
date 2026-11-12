#include "System/Entity/Components/CmpBounceBall.h"

#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/Particle/EmitterManager.h"
#include "System/Particle/SceneParticleServices.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

void CmpBounceBall::Initialize() {
  impactSequence_ = 0;
  impactCooldownRemaining_ = 0.0f;
}

void CmpBounceBall::SpawnImpactAt(const Math::Vector3 &worldPosition) {
  const std::string resolved = Particle::ResolveParticleJsonPath(impactJsonPath_);
  if (resolved.empty()) {
    return;
  }

  Particle::Emitter::EmitterManager *manager =
      Particle::EmitterManagerForActiveScene();
  if (manager == nullptr) {
    return;
  }

  const std::string emitterName = "ent_" + std::to_string(RefOwner().GetHash()) +
                                  "_impact_" + std::to_string(++impactSequence_);
  manager->AddOneShotEmitter(emitterName, resolved.c_str(), worldPosition);
}

void CmpBounceBall::Update() {
  FrameDeltaTime *frameDelta = EngineFrameDeltaTime();
  const float deltaSeconds =
      frameDelta != nullptr ? frameDelta->GetDeltaTime() : 0.0f;

  impactCooldownRemaining_ =
      std::max(0.0f, impactCooldownRemaining_ - deltaSeconds);

  Math::Vector3 &position = RefOwner().RefTransform().translate;
  velocity_.y += gravity_ * deltaSeconds;
  position += velocity_ * deltaSeconds;

  const float floorLimit = groundY_ + radius_;
  if (position.y <= floorLimit && velocity_.y < 0.0f) {
    position.y = floorLimit;
    velocity_.y = -velocity_.y * floorRestitution_;
    if (std::abs(velocity_.y) > 1.0f && impactCooldownRemaining_ <= 0.0f) {
      SpawnImpactAt({position.x, floorLimit, position.z});
      impactCooldownRemaining_ = impactCooldownSeconds_;
    }
  }

  const float leftLimit = leftWallX_ + radius_;
  const float rightLimit = rightWallX_ - radius_;
  if (position.x <= leftLimit && velocity_.x < 0.0f) {
    position.x = leftLimit;
    velocity_.x = std::abs(velocity_.x) * wallRestitution_;
    if (impactCooldownRemaining_ <= 0.0f) {
      SpawnImpactAt({leftWallX_, position.y, position.z});
      impactCooldownRemaining_ = impactCooldownSeconds_;
    }
  } else if (position.x >= rightLimit && velocity_.x > 0.0f) {
    position.x = rightLimit;
    velocity_.x = -std::abs(velocity_.x) * wallRestitution_;
    if (impactCooldownRemaining_ <= 0.0f) {
      SpawnImpactAt({rightWallX_, position.y, position.z});
      impactCooldownRemaining_ = impactCooldownSeconds_;
    }
  }
}

} // namespace Atrum
