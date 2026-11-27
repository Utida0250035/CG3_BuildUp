#include "System/AtomF/AtomFEnemyFactory.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFEnemyHitAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackFx.h"
#include "System/AtomF/CmpAtomFEnemyKnockbackPredictCapsule.h"
#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"
#include "System/AtomF/CmpAtomFEnemyShooter.h"
#include "System/AtomF/CmpAtomFEnemySpawnIn.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/AtomF/CmpAtomFEnemyWalkToPlayer.h"
#include "System/AtomF/CmpDrawAtomFTelegraph.h"
#include "System/AtomF/CmpDrawAtomFEnemyKnockbackPredictCapsule.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpParticleEmitter.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum::AtomF {

const EnemyVariantParameters *FindEnemyVariantByName(const std::string &name) {
  const auto &variants = ParametersRuntime::Instance().Enemy().variants;
  for (const EnemyVariantParameters &variant : variants) {
    if (variant.name == name) {
      return &variant;
    }
  }
  return nullptr;
}

std::unique_ptr<Entity> CreateEnemyEntity(const EnemyVariantParameters &variant,
                                          const Math::Vector3 &groundPosition,
                                          const int serialNumber) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float bodyRadius = ResolveEnemyBodyRadiusMeters();
  const std::string enemyName = "AtomFEnemy_" + std::to_string(serialNumber);

  std::unique_ptr<Entity> enemy = std::make_unique<Entity>(enemyName);
  enemy->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {groundPosition.x, bodyRadius, groundPosition.z};
  enemy->SetInitialTransform(initialTransform);

  enemy->AddUpdCmp<CmpAtomFEnemyVariant>();
  enemy->GetUpdCmp<CmpAtomFEnemyVariant>()->SetParameters(variant);

  enemy->AddUpdCmp<CmpAtomFEnemyKnockback>();
  enemy->AddUpdCmp<CmpAtomFEnemyChainExplosion>();
  enemy->AddUpdCmp<CmpAtomFEnemyTargetedPanic>();
  enemy->AddUpdCmp<CmpAtomFEnemyKnockbackPredictCapsule>();
  enemy->AddDrawCmp<CmpDrawAtomFEnemyKnockbackPredictCapsule>();
  enemy->AddUpdCmp<CmpAtomFEnemyWalkToPlayer>();
  enemy->AddUpdCmp<CmpAtomFEnemyPeriodicCharge>();
  enemy->AddUpdCmp<CmpAtomFEnemySpawnIn>();
  enemy->AddDrawCmp<CmpDrawAtomFTelegraph>();
  if (variant.shotIntervalSeconds > 0.0f) {
    enemy->AddUpdCmp<CmpAtomFEnemyShooter>();
  }

  enemy->AddDrawCmp<CmpMeshSphere>();
  CmpMeshSphere *mesh = enemy->GetDrawCmp<CmpMeshSphere>();
  if (mesh != nullptr) {
    mesh->SetRadius(bodyRadius);
    mesh->SetColor(
        Math::Vector4{variant.colorR, variant.colorG, variant.colorB, 1.0f});
    mesh->SetSubdivision(10);
    mesh->SetLightingEnabled(false);
    AttachMeshModel(*mesh, ResolveEnemyModelName());
  }

  AttachArenaContainmentForEnemy(*enemy);
  AttachEnemyHitSphere(*enemy);

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

} // namespace Atrum::AtomF
