#include "System/AtomF/AtomFBossSpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum::AtomF {

std::unique_ptr<Entity> CreateBossEntity(const Math::Vector3 &spawnPositionXZ) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const BossParameters &params = ResolveBossParameters();
  const float bodyRadius = params.bodyRadiusMeters;

  std::unique_ptr<Entity> boss = std::make_unique<Entity>("AtomFBoss");
  // enemy タグ: 自機への接触ダメージ・雑魚の連鎖爆発の接触対象になる。boss タグ: ボス専用の被ダメージ判定。
  boss->AddTag(tags.TagNameForRole("enemy"));
  boss->AddTag(tags.TagNameForRole("boss"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {spawnPositionXZ.x, bodyRadius,
                                spawnPositionXZ.z};
  boss->SetInitialTransform(initialTransform);

  boss->AddDrawCmp<CmpMeshSphere>();
  if (CmpMeshSphere *mesh = boss->GetDrawCmp<CmpMeshSphere>()) {
    mesh->SetRadius(0.0f); // 出現演出で膨らませる。
    mesh->SetColor(Math::Vector4{params.colorR, params.colorG, params.colorB, 1.0f});
    mesh->SetSubdivision(16);
    mesh->SetLightingEnabled(false);
    AttachMeshModel(*mesh, ResolveEnemyModelName());
  }

  boss->AddUpdCmp<CmpHitSphere>();
  if (CmpHitSphere *hit = boss->GetUpdCmp<CmpHitSphere>()) {
    hit->SetSpheres({{{}, bodyRadius}});
  }
  AttachArenaContainment(*boss, bodyRadius);

  boss->AddUpdCmp<CmpAtomFBoss>();

  boss->ResolveDependencies();
  boss->Initialize();
  return boss;
}

} // namespace Atrum::AtomF
