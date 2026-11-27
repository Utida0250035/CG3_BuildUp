#include "System/AtomF/AtomFBossSpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/AtomF/CmpDrawAtomFTelegraph.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum::AtomF {

void SpawnBoss(const Math::Vector3 &groundPosition) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const BossParameters &params = ParametersRuntime::Instance().Boss();
  const float radius = params.hitRadiusMeters;

  std::unique_ptr<Entity> boss = std::make_unique<Entity>("AtomFBoss");
  // ENEMY: 自機との接触でダメージ（CmpHealthHit の hurter）。BOSS: Lua の進行判定用。
  boss->AddTag(tags.TagNameForRole("enemy"));
  const std::string bossTag = tags.TagNameForRole("boss");
  if (!bossTag.empty()) {
    boss->AddTag(bossTag);
  }

  Math::TransformLH initialTransform{};
  initialTransform.translate = {groundPosition.x, radius, groundPosition.z};
  boss->SetInitialTransform(initialTransform);

  boss->AddUpdCmp<CmpAtomFBoss>();

  boss->AddDrawCmp<CmpMeshSphere>();
  if (CmpMeshSphere *mesh = boss->GetDrawCmp<CmpMeshSphere>()) {
    mesh->SetRadius(radius);
    mesh->SetColor(Math::Vector4{params.colorR, params.colorG, params.colorB, 1.0f});
    mesh->SetSubdivision(16);
    mesh->SetLightingEnabled(false);
    AttachMeshModel(*mesh, ResolveEnemyModelName());
  }
  boss->AddDrawCmp<CmpDrawAtomFTelegraph>();

  boss->AddUpdCmp<CmpHitSphere>();
  if (CmpHitSphere *hit = boss->GetUpdCmp<CmpHitSphere>()) {
    hit->SetSpheres({{{}, radius}});
  }
  AttachArenaContainment(*boss, radius);

  boss->ResolveDependencies();
  boss->Initialize();
  EnqueueSceneEntityAdopt(std::move(boss));
}

} // namespace Atrum::AtomF
