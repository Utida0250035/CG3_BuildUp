#include "System/AtomF/AtomFBossSpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/AtomF/CmpAtomFStageDirector.h"
#include "System/AtomF/CmpDrawAtomFBoss.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum::AtomF {

std::unique_ptr<Entity> CreateAtomFBossEntity() {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const BossParameters &params = ResolveBossParameters();

  std::unique_ptr<Entity> boss = std::make_unique<Entity>("AtomFBoss");
  // role "enemy": ゲーム進行（全滅判定）・エイム予測・壁/弾との当たりに乗せる。
  boss->AddTag(tags.TagNameForRole("enemy"));

  Math::TransformLH initialTransform{};
  initialTransform.translate = {0.0f, params.bodyRadiusMeters, 0.0f};
  boss->SetInitialTransform(initialTransform);

  boss->AddUpdCmp<CmpAtomFBoss>();

  boss->AddDrawCmp<CmpMeshSphere>();
  if (CmpMeshSphere *mesh = boss->GetDrawCmp<CmpMeshSphere>()) {
    mesh->SetRadius(0.01f);
    mesh->SetColor(Math::Vector4{params.colorR, params.colorG, params.colorB, 0.0f});
    mesh->SetSubdivision(24);
    mesh->SetLightingEnabled(false);
  }
  boss->AddDrawCmp<CmpDrawAtomFBoss>();

  AttachArenaContainment(*boss, params.bodyRadiusMeters);

  boss->AddUpdCmp<CmpHitSphere>();
  if (CmpHitSphere *hit = boss->GetUpdCmp<CmpHitSphere>()) {
    hit->SetSpheres({{{}, params.bodyRadiusMeters}});
    hit->SetIsActive(false); // 出現演出が終わるまで当たらない
  }

  boss->ResolveDependencies();
  boss->Initialize();
  return boss;
}

std::unique_ptr<Entity> CreateAtomFStageDirectorEntity() {
  std::unique_ptr<Entity> director =
      std::make_unique<Entity>("AtomFStageDirector");
  director->AddUpdCmp<CmpAtomFStageDirector>();
  director->ResolveDependencies();
  director->Initialize();
  return director;
}

} // namespace Atrum::AtomF
