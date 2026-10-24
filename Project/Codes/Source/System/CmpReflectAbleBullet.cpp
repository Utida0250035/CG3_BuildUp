#include "System/CmpReflectAbleBullet.h"
#include "System/CmpChildEntitys.h"
#include "System/CmpMeshSphere.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpLhRigidBody.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"
#include "System/PlaySe.h"
#include "System/StageBounds.h"

#include <unordered_set>

namespace Atrum {

namespace {

Math::Vector3 CombatEffectPoint(const Entity &a, const Entity &b) {
  return (a.GetWorldPosition() + b.GetWorldPosition()) * 0.5f;
}

/// 反射弾の表示色（CmpDrawHitSphere の水色と揃える）
constexpr Math::Vector4 kReflectedBulletColor{0.2f, 0.85f, 1.0f, 1.0f};

/// 管理弾（直線・メテオ・壁弾など）1発分の当たり・反射・ボス命中
bool ProcessManagedEnemyBullet(Entity *bullet, Entity &bulletOwner,
                               EntityType reflectorType) {
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

  if (bullet->GetEntityType() == EntityType::ENEMY_WEAPON) {
    // パリィを先に判定（ジャンプ無敵で自機 Hit が切れていてもメテオ等を跳ねられる）
    for (Entity *hitEntity : hitEntitys) {
      if (hitEntity->GetEntityType() != reflectorType) {
        continue;
      }

      bullet->SetEntityType(EntityType::PLAYER_ITEM);
      cmpLhRigidBody->RefVelocity() = {};
      cmpMeshSphere->SetColor(kReflectedBulletColor);
      bullet->RefTransform().translate = bullet->GetWorldPosition();
      bullet->SetParent(nullptr);
      PlaySe("./Resources/seParry.mp3");
      Particle::Combat::PlayParryReflectEffect(
          CombatEffectPoint(*bullet, *hitEntity));
      return false;
    }

    for (Entity *hitEntity : hitEntitys) {
      if (hitEntity->GetEntityType() != EntityType::PLAYER ||
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
  } else if (bullet->GetEntityType() == EntityType::PLAYER_ITEM) {
    constexpr float reflectedSpeed = 16.0f;
    const Math::Vector3 toOwner =
        bulletOwner.GetWorldPosition() - bullet->GetWorldPosition();
    cmpLhRigidBody->RefVelocity() = toOwner.Normalized() * reflectedSpeed;

    // オービター等（ENEMY_WEAPON）に触れただけでは弾を消さない
    for (Entity *hitEntity : hitEntitys) {
      if (hitEntity->GetEntityType() == EntityType::ENEMY_WEAPON) {
        return false;
      }
    }

    for (Entity *hitEntity : hitEntitys) {
      if (hitEntity->GetEntityType() != EntityType::ENEMY ||
          !hitEntity->HasUpdCmp<CmpHealthHit>()) {
        continue;
      }

      // ボス CmpHealthHit::Update は弾移動より先に走るため、ここでダメージを確定する
      CmpHealthHit *bossHealth = hitEntity->GetUpdCmp<CmpHealthHit>();
      bossHealth->TryTakeDamageFrom(bullet);

      Particle::Combat::PlayBossHitByReflectedBulletEffect(
          CombatEffectPoint(*bullet, *hitEntity));
      return true;
    }
  }

  return false;
}

/// ボス子实体の ENEMY_WEAPON（オービター等）
void ProcessAttachedEnemyWeapon(Entity *weapon) {
  if (weapon == nullptr || weapon->GetEntityType() != EntityType::ENEMY_WEAPON) {
    return;
  }

  CmpHitSphere *cmpHitSphere = weapon->GetUpdCmp<CmpHitSphere>();
  if (cmpHitSphere == nullptr) {
    return;
  }

  cmpHitSphere->Update();

  // 重なっている間は毎フレーム発火すると、反射演出の描画枠を埋め尽くす
  static std::unordered_set<Entity *> s_parryOverlapping;
  Entity *parryTarget = nullptr;
  for (Entity *hitEntity : cmpHitSphere->GetHitEntitys()) {
    if (hitEntity->GetEntityType() != EntityType::PLAYER_WEAPON) {
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
  PlaySe("./Resources/seParry.mp3");
}

bool IsOutOfStageBounds(const Math::Vector3 &position) {
  constexpr float kSpaceTopBuffer = 8.0f;
  constexpr float kSpaceSideBuffer = 8.0f;
  constexpr float kSpaceBottomBuffer = 0.25f;

  return position.x >= kRightEndX + kSpaceSideBuffer ||
         position.x <= kLeftEndX - kSpaceSideBuffer ||
         position.y >= kCeilHeight + kSpaceTopBuffer ||
         position.y <= kGroundHeight - kSpaceBottomBuffer;
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

void CmpReflectAbleBullet::Initialize() {
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

    if (IsOutOfStageBounds(bullet->GetWorldPosition())) {
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

    if (ProcessManagedEnemyBullet(bullet, RefOwner(), reflectorType_)) {
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
  reflectAbleBullets_.emplace_back(bullet);
}

} // namespace Atrum
