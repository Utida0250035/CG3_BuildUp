#include "System/AtomF/AtomFPlayerBulletSpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFPlayerBulletHitAttach.h"
#include "System/AtomF/CmpAtomFPlayerBulletEnemyHit.h"
#include "System/AtomF/CmpAtomFPlayerBulletMove.h"
#include "System/AtomF/CmpAtomFPlayerBulletTrail.h"
#include "System/AtomF/CmpDrawAtomFPlayerBulletTrail.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum::AtomF {

std::unique_ptr<Entity> CreatePlayerBulletEntity(
    const Math::Vector3 &worldSpawnPosition,
    const Math::Vector3 &aimDirectionXZUnit) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  std::unique_ptr<Entity> bullet =
      std::make_unique<Entity>("AtomFPlayerBullet");
  bullet->AddTag(tags.TagNameForRole("playerWeapon"));

  const float radius = ResolvePlayerBulletRadiusMeters();
  Math::TransformLH initialTransform{};
  initialTransform.translate = worldSpawnPosition;
  bullet->SetInitialTransform(initialTransform);

  bullet->AddUpdCmp<CmpAtomFPlayerBulletMove>();
  CmpAtomFPlayerBulletMove *move =
      bullet->GetUpdCmp<CmpAtomFPlayerBulletMove>();
  if (move != nullptr) {
    move->SetDirection(aimDirectionXZUnit);
  }

  bullet->AddDrawCmp<CmpMeshSphere>();
  CmpMeshSphere *mesh = bullet->GetDrawCmp<CmpMeshSphere>();
  if (mesh != nullptr) {
    mesh->SetRadius(radius);
    mesh->SetColor(Math::Vector4{0.95f, 0.92f, 0.3f, 1.0f});
    mesh->SetSubdivision(8);
  }

  AttachArenaContainmentForPlayerBullet(*bullet);
  AttachPlayerBulletHitSphere(*bullet);
  bullet->AddUpdCmp<CmpAtomFPlayerBulletTrail>();
  bullet->AddDrawCmp<CmpDrawAtomFPlayerBulletTrail>();
  bullet->AddUpdCmp<CmpAtomFPlayerBulletEnemyHit>();
  bullet->ResolveDependencies();
  bullet->Initialize();
  return bullet;
}

} // namespace Atrum::AtomF