#include "System/Entity/Components/CmpReflectAbleBullet.h"
#include "System/CombatBulletTuning.h"
#include "System/Entity/Components/CmpChildEntitys.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Particle/GameCombatParticleEffects.h"
#include "System/PlayerParryReach.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"

#include <cmath>
#include <unordered_set>

namespace Atrum {

namespace {

Math::Vector3 CombatEffectPoint(const Entity &a, const Entity &b) {
  return (a.GetWorldPosition() + b.GetWorldPosition()) * 0.5f;
}

/// 反射弾の表示色（CmpDrawHitSphere の水色と揃える）
constexpr Math::Vector4 kReflectedBulletColor{0.2f, 0.85f, 1.0f, 1.0f};

void PlayPlayerParrySe() {
  const std::string sePath = PlaySeDetail::ResolveExistingSePath(
      {"./Assets/Audios/sePlayerParry.mp3", "./Resources/sePlayerParry.mp3"});
  if (sePath.empty()) {
    return;
  }

  PlaySe(sePath);
}

void PlayBossHitByReflectedBulletSe() {
  const std::string sePath = PlaySeDetail::ResolveExistingSePath(
      {"./Assets/Audios/seBossHit.mp3", "./Resources/seBossHit.mp3"});
  if (sePath.empty()) {
    return;
  }

  PlaySe(sePath);
}

bool EntitiesOverlapByHitSphere(Entity &a, Entity &b) {
  CmpHitSphere *hitA = a.GetUpdCmp<CmpHitSphere>();
  CmpHitSphere *hitB = b.GetUpdCmp<CmpHitSphere>();
  if (hitA == nullptr || hitB == nullptr || !hitA->GetIsActive() ||
      !hitB->GetIsActive()) {
    return false;
  }

  const Math::Vector3 worldA = a.GetWorldPosition();
  const Math::Vector3 worldB = b.GetWorldPosition();

  for (const HitSphere &sphereA : hitA->GetSpheres()) {
    const Math::Vector3 centerA = worldA + sphereA.localOffset;

    for (const HitSphere &sphereB : hitB->GetSpheres()) {
      const Math::Vector3 centerB = worldB + sphereB.localOffset;
      const float reachRadius = sphereA.radius + sphereB.radius;

      if ((centerB - centerA).LengthSquare() <= reachRadius * reachRadius) {
        return true;
      }
    }
  }

  return false;
}

bool TryApplyReflectedBulletBossHit(Entity *bullet, Entity &bossEntity) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  if (bullet == nullptr || !tags.EntityHasRole(bossEntity, "enemy") ||
      !bossEntity.HasUpdCmp<CmpHealthHit>()) {
    return false;
  }

  if (!EntitiesOverlapByHitSphere(*bullet, bossEntity)) {
    return false;
  }

  CmpHealthHit *bossHealth = bossEntity.GetUpdCmp<CmpHealthHit>();
  if (!bossHealth->TryTakeDamageFrom(bullet)) {
    return false;
  }

  Particle::Combat::PlayBossHitByReflectedBulletEffect(
      CombatEffectPoint(*bullet, bossEntity));
  PlayBossHitByReflectedBulletSe();
  return true;
}

/// 管理弾（直線・メテオ・壁弾など）1発分の当たり・反射・ボス命中
bool ProcessManagedEnemyBullet(Entity *bullet, Entity &bulletOwner,
                               const uint64_t reflectorTagHash) {
  if (bullet == nullptr) {
    return false;
  }

  CmpHitSphere *cmpHitSphere = bullet->GetUpdCmp<CmpHitSphere>();
  CmpLhRigidBody *cmpLhRigidBody = bullet->GetUpdCmp<CmpLhRigidBody>();
  CmpMeshSphere *cmpMeshSphere = bullet->GetDrawCmp<CmpMeshSphere>();
  if (cmpHitSphere == nullptr || cmpLhRigidBody == nullptr ||
      cmpMeshSphere == nullptr) {
    return false;
  }

  const std::vector<Entity *> hitEntitys = cmpHitSphere->GetHitEntitys();

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  if (tags.EntityHasRole(*bullet, "enemyWeapon")) {
    // パリィを先に判定（ジャンプ無敵で自機 Hit が切れていてもメテオ等を跳ねられる）
    for (Entity *hitEntity : hitEntitys) {
      if (!hitEntity->HasTagHash(reflectorTagHash)) {
        continue;
      }

      tags.ApplyReflectParryTagConversion(*bullet);
      cmpLhRigidBody->RefVelocity() = {};
      cmpMeshSphere->SetColor(kReflectedBulletColor);
      bullet->SetParent(nullptr);
      PlayPlayerParrySe();
      Particle::Combat::PlayParryReflectEffect(
          CombatEffectPoint(*bullet, *hitEntity));
      return false;
    }

    for (Entity *hitEntity : hitEntitys) {
      if (!tags.EntityHasRole(*hitEntity, "player") ||
          !hitEntity->HasUpdCmp<CmpHealthHit>()) {
        continue;
      }

      CmpHealthHit *playerHealth = hitEntity->GetUpdCmp<CmpHealthHit>();
      if (playerHealth->TryTakeDamageFrom(bullet)) {
        Particle::Combat::PlayPlayerHitByEnemyBulletEffect(
            CombatEffectPoint(*bullet, *hitEntity));
        return true;
      }
    }
  } else if (tags.EntityHasRole(*bullet, "playerItem")) {
    constexpr float reflectedSpeed = 16.0f * kBulletSpeedGlobalScale;
    Math::Vector3 toOwner =
        bulletOwner.GetWorldPosition() - bullet->GetWorldPosition();
    const float toOwnerLengthSq = toOwner.LengthSquare();
    if (toOwnerLengthSq > 1.0e-8f) {
      cmpLhRigidBody->RefVelocity() =
          toOwner * (reflectedSpeed / std::sqrt(toOwnerLengthSq));
    }

    // ボス本体（弾の所有者）を優先。オービター接触だけでは命中判定を打ち切らない
    if (TryApplyReflectedBulletBossHit(bullet, bulletOwner)) {
      return true;
    }

    for (Entity *hitEntity : hitEntitys) {
      if (hitEntity == nullptr ||
          tags.EntityHasRole(*hitEntity, "enemyWeapon")) {
        continue;
      }

      if (!tags.EntityHasRole(*hitEntity, "enemy") ||
          !hitEntity->HasUpdCmp<CmpHealthHit>() || hitEntity == &bulletOwner) {
        continue;
      }

      if (TryApplyReflectedBulletBossHit(bullet, *hitEntity)) {
        return true;
      }
    }
  }

  return false;
}

std::unordered_set<Entity *> s_parryOverlapping{};

void ClearParryOverlapping() { s_parryOverlapping.clear(); }

/// ボス子实体の ENEMY_WEAPON（オービター等）
void ProcessAttachedEnemyWeapon(Entity *weapon) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  if (weapon == nullptr || !tags.EntityHasRole(*weapon, "enemyWeapon")) {
    return;
  }

  CmpHitSphere *cmpHitSphere = weapon->GetUpdCmp<CmpHitSphere>();
  if (cmpHitSphere == nullptr) {
    return;
  }

  cmpHitSphere->Update();

  // 重なっている間は毎フレーム発火すると、反射演出の描画枠を埋め尽くす
  Entity *parryTarget = nullptr;
  for (Entity *hitEntity : cmpHitSphere->GetHitEntitys()) {
    if (!hitEntity->HasTagHash(tags.DefaultParryReflectorTagHash())) {
      continue;
    }
    parryTarget = hitEntity;
    break;
  }

  if (parryTarget == nullptr) {
    s_parryOverlapping.erase(weapon);
    return;
  }

  if (s_parryOverlapping.contains(weapon)) {
    return;
  }

  s_parryOverlapping.insert(weapon);
  Particle::Combat::PlayParryReflectEffect(
      CombatEffectPoint(*weapon, *parryTarget));
  PlayPlayerParrySe();
}

void RefreshBulletHitSphere(Entity *bullet) {
  if (bullet == nullptr) {
    return;
  }
  if (CmpHitSphere *cmpHitSphere = bullet->GetUpdCmp<CmpHitSphere>()) {
    cmpHitSphere->Update();
  }
}

} // namespace

void CmpReflectAbleBullet::ResetAttachedWeaponParryOverlapTracking() {
  ClearParryOverlapping();
}

void CmpReflectAbleBullet::Initialize() {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  reflectorTagHash_ = tags.DefaultParryReflectorTagHash();

  for (auto &bullet : reflectAbleBullets_) {
    if (bullet) {
      bullet->Finalize();
    }
  }

  reflectAbleBullets_.clear();
}

void CmpReflectAbleBullet::Update() {
  size_t index = 0;

  while (index < reflectAbleBullets_.size()) {
    Entity *bullet = reflectAbleBullets_[index].get();
    if (!bullet) {
      reflectAbleBullets_.erase(reflectAbleBullets_.begin() +
                                static_cast<std::ptrdiff_t>(index));
      continue;
    }

    if (bullet->GetState() != Entity::State::Active &&
        bullet->GetState() != Entity::State::Sleep) {
      bullet->Finalize();
      reflectAbleBullets_.erase(reflectAbleBullets_.begin() +
                                static_cast<std::ptrdiff_t>(index));
      continue;
    }

    bullet->Update();

    EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  if (tags.EntityHasRole(*bullet, "enemyWeapon")) {
      bullet->RefTransform().translate.z = 0.0f;
      if (CmpLhRigidBody *body = bullet->GetUpdCmp<CmpLhRigidBody>()) {
        body->RefVelocity().z = 0.0f;
      }
    }

    if (IsOutsideCombatAreaAabb(bullet->GetWorldPosition())) {
      bullet->Finalize();
      reflectAbleBullets_.erase(reflectAbleBullets_.begin() +
                                static_cast<std::ptrdiff_t>(index));
      continue;
    }

    if (tags.EntityHasRole(*bullet, "enemyWeapon") &&
        IsOutsidePlayerParryReachAabb(bullet->GetWorldPosition())) {
      bullet->Finalize();
      reflectAbleBullets_.erase(reflectAbleBullets_.begin() +
                                static_cast<std::ptrdiff_t>(index));
      continue;
    }

    ++index;
  }
}

void CmpReflectAbleBullet::LateResolveCombatHits() {
  size_t index = 0;

  while (index < reflectAbleBullets_.size()) {
    Entity *bullet = reflectAbleBullets_[index].get();
    if (!bullet) {
      reflectAbleBullets_.erase(reflectAbleBullets_.begin() +
                                static_cast<std::ptrdiff_t>(index));
      continue;
    }

    if (bullet->GetState() != Entity::State::Active &&
        bullet->GetState() != Entity::State::Sleep) {
      bullet->Finalize();
      reflectAbleBullets_.erase(reflectAbleBullets_.begin() +
                                static_cast<std::ptrdiff_t>(index));
      continue;
    }

    RefreshBulletHitSphere(bullet);

    if (ProcessManagedEnemyBullet(bullet, RefOwner(), reflectorTagHash_)) {
      bullet->Finalize();
      reflectAbleBullets_.erase(reflectAbleBullets_.begin() +
                                static_cast<std::ptrdiff_t>(index));
      continue;
    }

    if (bullet->GetState() == Entity::State::Sleep) {
      ++index;
      continue;
    }

    ++index;
  }

  if (CmpChildEntitys *childEntitys = RefOwner().GetUpdCmp<CmpChildEntitys>()) {
    for (const std::unique_ptr<Entity> &child : childEntitys->RefChilds()) {
      ProcessAttachedEnemyWeapon(child.get());
    }
  }
}

void CmpReflectAbleBullet::Shot(Entity *bullet) {
  if (bullet == nullptr) {
    return;
  }

  std::unique_ptr<Entity> owned(bullet);
  EntityTagRegistry::Instance().EnsureLoaded();
  if (EntityTagRegistry::Instance().EntityHasRole(*owned, "enemyWeapon") &&
      IsOutsidePlayerParryReachAabb(owned->GetWorldPosition())) {
    owned->Finalize();
    return;
  }

  reflectAbleBullets_.emplace_back(std::move(owned));
}

} // namespace Atrum
