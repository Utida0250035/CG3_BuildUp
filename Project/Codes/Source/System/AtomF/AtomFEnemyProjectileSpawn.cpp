#include "System/AtomF/AtomFEnemyProjectileSpawn.h"

#include "Math/Transform.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/CmpAtomFEnemyProjectile.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

#include <cmath>

namespace Atrum::AtomF {

void SpawnEnemyProjectile(const Math::Vector3 &position,
                          const Math::Vector3 &directionXZ,
                          const float speedMetersPerSecond,
                          const float radiusMeters,
                          const Math::Vector4 &color) {
  Math::Vector3 dir = directionXZ;
  dir.y = 0.0f;
  const float lengthSq = dir.LengthSquare();
  if (lengthSq < 1e-8f) {
    return;
  }
  dir *= (1.0f / std::sqrt(lengthSq));

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  std::unique_ptr<Entity> bullet = std::make_unique<Entity>("AtomFEnemyShot");
  bullet->AddTag(tags.TagNameForRole("enemyWeapon"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {position.x, radiusMeters, position.z};
  bullet->SetInitialTransform(initialTransform);

  bullet->AddUpdCmp<CmpHitSphere>();
  if (CmpHitSphere *hit = bullet->GetUpdCmp<CmpHitSphere>()) {
    hit->SetSpheres({{{}, radiusMeters}});
  }

  bullet->AddUpdCmp<CmpAtomFEnemyProjectile>();
  if (CmpAtomFEnemyProjectile *projectile =
          bullet->GetUpdCmp<CmpAtomFEnemyProjectile>()) {
    projectile->SetVelocity(dir * speedMetersPerSecond);
    projectile->SetRadiusMeters(radiusMeters);
  }

  bullet->AddDrawCmp<CmpMeshSphere>();
  if (CmpMeshSphere *mesh = bullet->GetDrawCmp<CmpMeshSphere>()) {
    mesh->SetRadius(radiusMeters);
    mesh->SetColor(color);
    mesh->SetSubdivision(8);
    mesh->SetLightingEnabled(false);
  }

  bullet->ResolveDependencies();
  bullet->Initialize();
  EnqueueSceneEntityAdopt(std::move(bullet));
}

} // namespace Atrum::AtomF
