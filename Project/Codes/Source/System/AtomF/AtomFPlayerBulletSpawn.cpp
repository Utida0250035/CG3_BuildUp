#include "System/AtomF/AtomFPlayerBulletSpawn.h"

#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFPlayerBulletHitAttach.h"
#include "System/AtomF/CmpAtomFPlayerBulletArenaWallHit.h"
#include "System/AtomF/CmpAtomFPlayerBulletEnemyHit.h"
#include "System/AtomF/CmpAtomFPlayerBulletMove.h"
#include "System/AtomF/CmpAtomFPlayerBulletPredictCapsule.h"
#include "System/AtomF/CmpAtomFPlayerBulletTrail.h"
#include "System/AtomF/CmpDrawAtomFPlayerBulletPredictCapsule.h"
#include "System/AtomF/CmpDrawAtomFPlayerBulletTrail.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

#include <cmath>

namespace Atrum::AtomF {

std::unique_ptr<Entity> CreatePlayerBulletEntity(
    const Math::Vector3 &aimDirectionXZUnit,
    const Math::Vector3 &worldSpawnPosition) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  std::unique_ptr<Entity> bullet =
      std::make_unique<Entity>("AtomFPlayerBullet");
  bullet->AddTag(tags.TagNameForRole("playerWeapon"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = worldSpawnPosition;
  // モデルの先端（+Z）を飛ぶ向きへ合わせる。
  initialTransform.quaternion = Math::Quaternion::FromAxisAngle(
      Math::Vector3{0.0f, 1.0f, 0.0f},
      std::atan2(aimDirectionXZUnit.x, aimDirectionXZUnit.z));
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
    mesh->SetRadius(ResolvePlayerBulletRadiusMeters());
    mesh->SetColor(Math::Vector4{0.95f, 0.92f, 0.3f, 1.0f});
    mesh->SetSubdivision(8);
    mesh->SetLightingEnabled(false);
    AttachMeshModel(*mesh, ResolvePlayerBulletModelName());
  }

  AttachArenaContainmentForPlayerBullet(*bullet);
  SyncPlayerBulletHitRadiusA(*bullet);
  bullet->AddUpdCmp<CmpAtomFPlayerBulletPredictCapsule>();
  bullet->AddDrawCmp<CmpDrawAtomFPlayerBulletPredictCapsule>();
  bullet->AddUpdCmp<CmpAtomFPlayerBulletTrail>();
  bullet->AddDrawCmp<CmpDrawAtomFPlayerBulletTrail>();
  bullet->AddUpdCmp<CmpAtomFPlayerBulletEnemyHit>();
  bullet->AddUpdCmp<CmpAtomFPlayerBulletArenaWallHit>();
  bullet->ResolveDependencies();
  bullet->Initialize();
  return bullet;
}

} // namespace Atrum::AtomF