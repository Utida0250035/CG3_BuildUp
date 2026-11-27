#include "System/AtomF/AtomFEnemyProjectileSpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/CmpAtomFEnemyProjectile.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

#include <memory>

namespace Atrum::AtomF {

void SpawnEnemyProjectile(const Math::Vector3 &worldPosition,
                          const Math::Vector3 &directionXZ, const float colorR,
                          const float colorG, const float colorB,
                          const float speedScale) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const EnemyProjectileParameters &params = ResolveEnemyProjectileParameters();

  std::unique_ptr<Entity> bullet =
      std::make_unique<Entity>("AtomFEnemyProjectile");
  bullet->AddTag(tags.TagNameForRole("enemyWeapon"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = worldPosition;
  bullet->SetInitialTransform(initialTransform);

  bullet->AddUpdCmp<CmpHitSphere>();
  if (CmpHitSphere *hit = bullet->GetUpdCmp<CmpHitSphere>()) {
    hit->SetSpheres({{{}, params.radiusMeters}});
  }

  bullet->AddUpdCmp<CmpAtomFEnemyProjectile>();
  if (CmpAtomFEnemyProjectile *move =
          bullet->GetUpdCmp<CmpAtomFEnemyProjectile>()) {
    move->SetDirection(directionXZ);
    move->SetSpeedMetersPerSecond(params.speedMetersPerSecond * speedScale);
    move->SetRadiusMeters(params.radiusMeters);
    move->SetLifetimeSeconds(params.lifetimeSeconds);
  }

  bullet->AddDrawCmp<CmpMeshSphere>();
  if (CmpMeshSphere *mesh = bullet->GetDrawCmp<CmpMeshSphere>()) {
    mesh->SetRadius(params.radiusMeters);
    mesh->SetColor(Math::Vector4{colorR, colorG, colorB, 1.0f});
    mesh->SetSubdivision(8);
    mesh->SetLightingEnabled(false);
  }

  bullet->ResolveDependencies();
  bullet->Initialize();
  EnqueueSceneEntityAdopt(std::move(bullet));
}

} // namespace Atrum::AtomF
