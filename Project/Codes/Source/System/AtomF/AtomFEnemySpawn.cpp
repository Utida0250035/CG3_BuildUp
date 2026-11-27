#include "System/AtomF/AtomFEnemySpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFEnemyHitAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackFx.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackPredictCapsule.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/AtomF/CmpAtomFEnemyWalkToPlayer.h"
#include "System/AtomF/CmpDrawAtomFEnemyKnockbackPredictCapsule.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpParticleEmitter.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"

#include <cmath>

namespace Atrum::AtomF {

std::unique_ptr<Entity> CreateAtomFEnemyEntity(const Math::Vector3 &xzPosition,
                                               const int index) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float bodyRadius = AtomF::ResolveEnemyBodyRadiusMeters();

  const std::string enemyName = "AtomFEnemy_" + std::to_string(index);
  std::unique_ptr<Entity> enemy = std::make_unique<Entity>(enemyName);
  enemy->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {xzPosition.x, bodyRadius, xzPosition.z};
  enemy->SetInitialTransform(initialTransform);

  // 種類（variants）を index % 件数 で割り当てる。未定義なら従来の単一種。
  AtomF::EnemyVariantParameters variant{};
  if (AtomF::ParametersRuntime::Instance().IsLoaded()) {
    const auto &variants = AtomF::ParametersRuntime::Instance().Enemy().variants;
    if (!variants.empty()) {
      variant = variants[static_cast<std::size_t>(index) % variants.size()];
    }
  }
  enemy->AddUpdCmp<CmpAtomFEnemyVariant>();
  enemy->GetUpdCmp<CmpAtomFEnemyVariant>()->SetParameters(variant);

  enemy->AddUpdCmp<CmpAtomFEnemyKnockback>();
  enemy->AddUpdCmp<CmpAtomFEnemyChainExplosion>();
  enemy->AddUpdCmp<CmpAtomFEnemyTargetedPanic>();
  enemy->AddUpdCmp<CmpAtomFEnemyKnockbackPredictCapsule>();
  enemy->AddDrawCmp<CmpDrawAtomFEnemyKnockbackPredictCapsule>();
  enemy->AddUpdCmp<CmpAtomFEnemyWalkToPlayer>();
  enemy->AddUpdCmp<CmpAtomFEnemyPeriodicCharge>();

  enemy->AddDrawCmp<CmpMeshSphere>();
  CmpMeshSphere *mesh = enemy->GetDrawCmp<CmpMeshSphere>();
  if (mesh != nullptr) {
    mesh->SetRadius(bodyRadius);
    mesh->SetColor(Math::Vector4{variant.colorR, variant.colorG, variant.colorB, 1.0f});
    mesh->SetSubdivision(10);
    mesh->SetLightingEnabled(false);
    AtomF::AttachMeshModel(*mesh, AtomF::ResolveEnemyModelName());
  }

  AtomF::AttachArenaContainmentForEnemy(*enemy);
  AtomF::AttachEnemyHitSphere(*enemy);

  // 吹き飛び演出: 子エンティティにエミッタを付け、敵の行列を親として継承する。
  std::unique_ptr<Entity> trailChild =
      std::make_unique<Entity>(enemyName + "_KnockbackTrail");
  trailChild->AddUpdCmp<CmpParticleEmitter>();
  CmpParticleEmitter *trailEmitter = trailChild->GetUpdCmp<CmpParticleEmitter>();
  trailEmitter->SetJsonFilePath("atomf_knockback_trail.json");
  trailEmitter->SetInheritOwnerMatrix(true);
  trailEmitter->SetEmitEnabled(false);
  // 発生範囲の幅を敵の直径と同程度にする（円の半径 = 敵の半径）。
  trailEmitter->SetEmitCircleRadiusOverride(bodyRadius);

  std::unique_ptr<Entity> auraChild =
      std::make_unique<Entity>(enemyName + "_KnockbackAura");
  auraChild->AddUpdCmp<CmpParticleEmitter>();
  CmpParticleEmitter *auraEmitter = auraChild->GetUpdCmp<CmpParticleEmitter>();
  auraEmitter->SetJsonFilePath("atomf_knockback_aura.json");
  auraEmitter->SetInheritOwnerMatrix(true);
  auraEmitter->SetEmitEnabled(false);

  enemy->AddUpdCmp<CmpChildEntitys>();
  enemy->GetUpdCmp<CmpChildEntitys>()->AddChilds(
      {trailChild.release(), auraChild.release()});
  enemy->AddUpdCmp<CmpAtomFEnemyKnockbackFx>();
  CmpAtomFEnemyKnockbackFx *knockbackFx =
      enemy->GetUpdCmp<CmpAtomFEnemyKnockbackFx>();
  knockbackFx->SetTrailEmitter(trailEmitter);
  knockbackFx->SetAuraEmitter(auraEmitter);

  enemy->ResolveDependencies();
  enemy->Initialize();
  return enemy;
}

namespace {
int NextWaveSerial() {
  static int waveSerial = 1000; // 初期群れ(0..)と名前が衝突しないようにする
  return waveSerial++;
}
} // namespace

void SpawnAtomFEnemyAt(const Math::Vector3 &xzPosition) {
  EnqueueSceneEntityAdopt(CreateAtomFEnemyEntity(xzPosition, NextWaveSerial()));
}

std::size_t SpawnAtomFEnemyWave(const int count, const float spawnRadiusMeters,
                                const float angleOffsetRadians) {
  if (count <= 0) {
    return 0;
  }
  constexpr float kTwoPi = 6.28318530718f;
  std::size_t spawned = 0;
  for (int i = 0; i < count; ++i) {
    const float angle = angleOffsetRadians + (kTwoPi * static_cast<float>(i)) /
                                                 static_cast<float>(count);
    const Math::Vector3 position{std::cos(angle) * spawnRadiusMeters, 0.0f,
                                 std::sin(angle) * spawnRadiusMeters};
    EnqueueSceneEntityAdopt(CreateAtomFEnemyEntity(position, NextWaveSerial()));
    ++spawned;
  }
  return spawned;
}

} // namespace Atrum::AtomF
