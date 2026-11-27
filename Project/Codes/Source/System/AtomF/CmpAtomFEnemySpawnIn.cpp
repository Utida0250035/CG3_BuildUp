#include "System/AtomF/CmpAtomFEnemySpawnIn.h"

#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpDrawAtomFTelegraph.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

void CmpAtomFEnemySpawnIn::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  fullRadius_ = AtomF::ResolveEnemyBodyRadiusMeters();
}

void CmpAtomFEnemySpawnIn::Initialize() {
  elapsedSeconds_ = 0.0f;
  finished_ = false;
  effectPlayed_ = false;
  if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
    if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
      hit->SetIsActive(false);
    }
  }
}

void CmpAtomFEnemySpawnIn::Update() {
  if (finished_ || frameDeltaTime_ == nullptr) {
    return;
  }

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
    if (const CmpAtomFEnemyChainExplosion *chain =
            RefOwner().GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      finished_ = true; // 爆発演出（縮小）に任せる
      return;
    }
  }

  if (!effectPlayed_) {
    effectPlayed_ = true;
    Particle::Combat::PlayNamedOneShotEffect("atomf_enemy_spawn.json",
                                             RefOwner().GetWorldPosition());
  }

  elapsedSeconds_ += frameDeltaTime_->GetDeltaTime();
  const float t = std::clamp(elapsedSeconds_ / durationSeconds_, 0.0f, 1.0f);
  // 終盤で少し行き過ぎてから戻る（弾むような実体化）。
  const float overshoot = 1.0f + 0.25f * std::sin(t * 3.14159265f);
  const float scale = t * overshoot;

  if (RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    if (CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>()) {
      mesh->SetRadius(fullRadius_ * scale);
    }
  }
  if (RefOwner().HasDrawCmp<CmpDrawAtomFTelegraph>()) {
    if (CmpDrawAtomFTelegraph *telegraph =
            RefOwner().GetDrawCmp<CmpDrawAtomFTelegraph>()) {
      telegraph->Clear();
      if (t < 1.0f) {
        Math::Vector3 center = RefOwner().GetWorldPosition();
        center.y = 0.0f;
        telegraph->AddRing(center, fullRadius_ * (2.8f - 1.8f * t), 0.05f,
                      Math::Vector4{0.5f, 0.9f, 1.0f, 0.8f * (1.0f - t)});
      }
    }
  }

  if (t >= 1.0f) {
    finished_ = true;
    if (RefOwner().HasDrawCmp<CmpMeshSphere>()) {
      if (CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>()) {
        mesh->SetRadius(fullRadius_);
      }
    }
    // 実体化後に当たり判定を戻す。
    if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
      if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
        hit->SetIsActive(true);
      }
    }
  }
}

} // namespace Atrum
