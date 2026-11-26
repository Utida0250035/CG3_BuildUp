#include "System/AtomF/CmpAtomFPlayerInvincibleBlink.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

float CmpAtomFPlayerInvincibleBlink::EvaluateAlpha(const float elapsedSeconds,
                                                   const float pulseSeconds,
                                                   const int pulseCount,
                                                   const float minAlpha) {
  const float blinkTotalSeconds =
      pulseSeconds * static_cast<float>(pulseCount);
  if (pulseSeconds <= 0.0f || elapsedSeconds < 0.0f ||
      elapsedSeconds >= blinkTotalSeconds) {
    return 1.0f;
  }
  const float phase01 = std::fmod(elapsedSeconds, pulseSeconds) / pulseSeconds;
  return 1.0f - (1.0f - minAlpha) * phase01;
}

void CmpAtomFPlayerInvincibleBlink::ResolveDependence() {
  health_ = RefOwner().GetUpdCmp<CmpHealthHit>();
  mesh_ = RefOwner().GetDrawCmp<CmpMeshSphere>();
}

void CmpAtomFPlayerInvincibleBlink::Update() {
  if (health_ == nullptr || mesh_ == nullptr) {
    return;
  }

  float alpha = 1.0f;
  if (health_->IsInvincible()) {
    const AtomF::PlayerInvincibilityParameters &params =
        AtomF::ResolvePlayerInvincibilityParameters();
    const float elapsedSeconds =
        health_->GetInvincibleTime() - health_->GetInvincibleRemainingSeconds();
    alpha = EvaluateAlpha(elapsedSeconds, params.blinkPulseSeconds,
                          params.blinkPulseCount, params.blinkMinAlpha);
  }

  Math::Vector4 color = mesh_->GetColor();
  color.w = alpha;
  mesh_->SetColor(color);
}

} // namespace Atrum
