#include "System/AtomF/CmpAtomFEnemyKnockbackFx.h"

#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/Entity/Components/CmpParticleEmitter.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpAtomFEnemyKnockbackFx::ResolveDependence() {
  knockback_ = RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()
                   ? RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>()
                   : nullptr;
}

void CmpAtomFEnemyKnockbackFx::SetEmitting(const bool emitting) {
  if (emitting_ == emitting) {
    return;
  }
  emitting_ = emitting;
  if (trailEmitter_ != nullptr) {
    trailEmitter_->SetEmitEnabled(emitting);
  }
  if (auraEmitter_ != nullptr) {
    auraEmitter_->SetEmitEnabled(emitting);
  }
}

void CmpAtomFEnemyKnockbackFx::Update() {
  if (knockback_ == nullptr) {
    return;
  }

  const bool active = knockback_->HasActiveKnockback();
  SetEmitting(active);
  if (!active || trailEmitter_ == nullptr) {
    return;
  }

  // 吹き飛び方向の反対（ワールド）を、敵ローカル（子エミッタの親空間）へ変換する。
  Math::Vector3 backWorld = -knockback_->GetVelocity();
  backWorld.y = 0.0f;
  if (backWorld.LengthSquare() < 1e-8f) {
    return;
  }
  backWorld.Normalize();

  const Math::Vector3 originWorld = RefOwner().GetWorldPosition();
  const Math::Matrix4x4 worldInverse = RefOwner().GetWorldMatrix().Inversed();
  Math::Vector3 forward = worldInverse.Transform(originWorld + backWorld) -
                          worldInverse.Transform(originWorld);
  if (forward.LengthSquare() < 1e-8f) {
    return;
  }
  forward.Normalize();

  // エミッタのローカル +Z を後方へ向ける（発生円は XY 平面＝進行方向に垂直）。
  Math::Vector3 right = Math::Vector3::UpLh().CrossLh(forward);
  if (right.LengthSquare() < 1e-5f) {
    return;
  }
  right.Normalize();
  Math::Vector3 up = forward.CrossLh(right);
  up.Normalize();

  Math::TransformLH local{};
  local.quaternion = Math::Quaternion::FromAxes(right, up, forward);
  trailEmitter_->SetLocalTransform(local);
}

void CmpAtomFEnemyKnockbackFx::Finalize() {
  for (Entity *child : RefOwner().GetChilds()) {
    if (child != nullptr) {
      child->Finalize();
    }
  }
}

} // namespace Atrum
