#include "System/AtomF/CmpAtomFPlayerBulletTrail.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpAtomFPlayerBulletTrail::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  trailDurationSeconds_ = AtomF::ResolvePlayerBulletTrailSeconds();
}

void CmpAtomFPlayerBulletTrail::BeginLingerPhase() {
  lingerPhase_ = true;
  if (CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>()) {
    mesh->SetRadius(0.0f);
  }
}

void CmpAtomFPlayerBulletTrail::CompleteLingerDestroyIfReady() {
  if (!lingerPhase_ || destroyRequested_) {
    return;
  }
  if (!samples_.empty()) {
    return;
  }
  destroyRequested_ = true;
  RefOwner().SetParent(nullptr);
  RefOwner().Finalize();
}

void CmpAtomFPlayerBulletTrail::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  const float dt = frameDeltaTime_->GetDeltaTime();

  for (AtomFPlayerBulletTrailSample &sample : samples_) {
    sample.ageSeconds += dt;
  }

  while (!samples_.empty() &&
         samples_.front().ageSeconds > trailDurationSeconds_) {
    samples_.erase(samples_.begin());
  }

  if (!lingerPhase_) {
    AtomFPlayerBulletTrailSample sample{};
    sample.ageSeconds = 0.0f;
    sample.worldPosition = RefOwner().GetWorldPosition();
    samples_.push_back(sample);
  }

  if (lingerPhase_) {
    CompleteLingerDestroyIfReady();
  }
}

void CmpAtomFPlayerBulletTrail::Finalize() {
  samples_.clear();
}

} // namespace Atrum