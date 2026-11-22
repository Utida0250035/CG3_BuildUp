#include "System/Entity/Components/CmpBallBounceImpactFx.h"

#include "Engine/EngineAccess.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/Particle/EmitterManager.h"
#include "System/Particle/SceneParticleServices.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

void CmpBallBounceImpactFx::Initialize() {
  impactSequence_ = 0;
  impactCooldownRemaining_ = 0.0f;
}

void CmpBallBounceImpactFx::Update() {
  FrameDeltaTime *frameDelta = EngineFrameDeltaTime();
  const float deltaSeconds =
      frameDelta != nullptr ? frameDelta->GetDeltaTime() : 0.0f;
  impactCooldownRemaining_ =
      std::max(0.0f, impactCooldownRemaining_ - deltaSeconds);
}

void CmpBallBounceImpactFx::TrySpawnImpactAt(
    const Math::Vector3 &worldPosition, const float verticalSpeedMagnitude,
    const bool enforceVerticalSpeedGate) {
  if (impactCooldownRemaining_ > 0.0f) {
    return;
  }
  if (enforceVerticalSpeedGate && std::abs(verticalSpeedMagnitude) <= 1.0f) {
    return;
  }
  SpawnImpactAt(worldPosition);
  impactCooldownRemaining_ = impactCooldownSeconds_;
}

void CmpBallBounceImpactFx::SpawnImpactAt(const Math::Vector3 &worldPosition) {
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

} // namespace Atrum