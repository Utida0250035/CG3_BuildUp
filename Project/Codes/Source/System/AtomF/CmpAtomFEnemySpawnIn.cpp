#include "System/AtomF/CmpAtomFEnemySpawnIn.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

namespace {

constexpr float kSpawnInSeconds = 0.5f;

/// 0→1 を行き過ぎて戻る（オーバーシュート）カーブ。
float EaseOutBack(const float t) {
  constexpr float kC1 = 1.70158f;
  constexpr float kC3 = kC1 + 1.0f;
  const float u = t - 1.0f;
  return 1.0f + kC3 * u * u * u + kC1 * u * u;
}

} // namespace

void CmpAtomFEnemySpawnIn::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  baseRadius_ = AtomF::ResolveEnemyBodyRadiusMeters();
}

void CmpAtomFEnemySpawnIn::Update() {
  if (finished_ || frameDeltaTime_ == nullptr) {
    return;
  }
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
    if (const CmpAtomFEnemyChainExplosion *chain =
            RefOwner().GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      finished_ = true;
      return;
    }
  }
  if (!RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    finished_ = true;
    return;
  }
  CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>();
  if (mesh == nullptr) {
    finished_ = true;
    return;
  }

  elapsedSeconds_ += frameDeltaTime_->GetDeltaTime();
  const float t = std::clamp(elapsedSeconds_ / kSpawnInSeconds, 0.0f, 1.0f);
  mesh->SetRadius(baseRadius_ * std::max(0.0f, EaseOutBack(t)));
  if (t >= 1.0f) {
    mesh->SetRadius(baseRadius_);
    finished_ = true;
  }
}

} // namespace Atrum
