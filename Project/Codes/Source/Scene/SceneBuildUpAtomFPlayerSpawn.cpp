#include "Scene/SceneBuildUpAtomFPlayerSpawn.h"

#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaContainmentAttach.h"
#include "System/AtomF/AtomFMeshModelAttach.h"
#include "System/AtomF/AtomFPlayerHealthAttach.h"
#include "System/AtomF/AtomFPlayerHitAttach.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/CmpAtomFPlayerAim.h"
#include "System/AtomF/CmpAtomFPlayerAimCapsule.h"
#include "System/AtomF/CmpAtomFPlayerAimCapsuleHitSync.h"
#include "System/AtomF/CmpDrawAtomFPlayerAimCapsule.h"
#include "System/AtomF/CmpAtomFPlayerFire.h"
#include "System/AtomF/CmpAtomFPlayerInvincibleBlink.h"
#include "System/AtomF/CmpAtomFPlayerMove.h"
#include "System/AtomF/CmpAtomFPlayerShotCooldown.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum {

void AppendAtomFPlayerIfScene(
    const std::string &sceneJsonPath,
    std::vector<std::unique_ptr<Entity>> &entities) {
  if (!AtomF::PathEndsWithAtomFJson(sceneJsonPath)) {
    return;
  }

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  // 初回の発射で読込待ちが出ないよう、自機弾のモデルはシーン入場時に読む。
  AtomF::LoadMeshModel(AtomF::ResolvePlayerBulletModelName());

  std::unique_ptr<Entity> player = std::make_unique<Entity>("AtomFPlayer");
  player->AddTag(tags.TagNameForRole("player"));

  const float bodyRadius = AtomF::ResolvePlayerBodyRadiusMeters();
  Math::TransformLH initialTransform{};
  initialTransform.translate = {0.0f, bodyRadius, 0.0f};
  player->SetInitialTransform(initialTransform);

  player->AddUpdCmp<CmpAtomFPlayerAim>();
  player->AddUpdCmp<CmpAtomFPlayerAimCapsule>();
  player->AddUpdCmp<CmpAtomFPlayerAimCapsuleHitSync>();
  player->AddDrawCmp<CmpDrawAtomFPlayerAimCapsule>();
  player->AddUpdCmp<CmpAtomFPlayerMove>();
  player->AddUpdCmp<CmpAtomFPlayerShotCooldown>();
  player->AddUpdCmp<CmpAtomFPlayerFire>();
  player->AddDrawCmp<CmpMeshSphere>();
  CmpMeshSphere *mesh = player->GetDrawCmp<CmpMeshSphere>();
  if (mesh != nullptr) {
    mesh->SetRadius(bodyRadius);
    mesh->SetColor(Math::Vector4{0.25f, 0.55f, 0.95f, 1.0f});
    mesh->SetSubdivision(12);
    mesh->SetLightingEnabled(false);
  }

  AtomF::AttachArenaContainmentForPlayer(*player);
  AtomF::AttachPlayerHitSphere(*player);
  AtomF::AttachPlayerHealthHit(*player);
  player->AddUpdCmp<CmpAtomFPlayerInvincibleBlink>();
  player->ResolveDependencies();
  player->Initialize();
  entities.push_back(std::move(player));
}

} // namespace Atrum