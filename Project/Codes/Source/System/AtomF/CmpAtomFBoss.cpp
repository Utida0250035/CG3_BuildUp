#include "System/AtomF/CmpAtomFBoss.h"

#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFEnemyExplosionKnockbackSpread.h"
#include "System/AtomF/AtomFEnemySpawn.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Particle/GameCombatParticleEffects.h"
#include "System/Scripting/ScriptSceneContext.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

namespace {

constexpr float kTwoPi = 6.28318530718f;

uint32_t NextEffectSequence() {
  static uint32_t sequence = 50000U; // 雑魚の爆発（連番の若い側）と被らせない
  return ++sequence;
}

float Saturate(const float v) { return std::clamp(v, 0.0f, 1.0f); }

float EaseOutCubic(const float t) {
  const float u = 1.0f - Saturate(t);
  return 1.0f - u * u * u;
}

/// ボスの本体が出現する床上の高さ（球の中心）。
float BossCenterY(const float radius) { return radius; }

} // namespace

void CmpAtomFBoss::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  params_ = AtomF::ResolveBossParameters();
  health_ = params_.maxHealth;
}

bool CmpAtomFBoss::IsTargetable() const {
  switch (state_) {
  case State::Idle:
  case State::TelegraphCharge:
  case State::Charging:
  case State::TelegraphSummon:
  case State::TelegraphNova:
  case State::Stunned:
    return true;
  default:
    return false;
  }
}

float CmpAtomFBoss::HealthRatio() const {
  if (params_.maxHealth <= 0) {
    return 0.0f;
  }
  return Saturate(static_cast<float>(health_) /
                  static_cast<float>(params_.maxHealth));
}

float CmpAtomFBoss::StateProgress() const {
  if (stateDurationSeconds_ <= 1e-5f) {
    return 1.0f;
  }
  return Saturate(stateElapsedSeconds_ / stateDurationSeconds_);
}

float CmpAtomFBoss::NextRandom01() {
  rngState_ ^= rngState_ << 13;
  rngState_ ^= rngState_ >> 17;
  rngState_ ^= rngState_ << 5;
  return static_cast<float>(rngState_ & 0xFFFFFFu) /
         static_cast<float>(0x1000000u);
}

Math::Vector3 CmpAtomFBoss::DirectionToPlayerXZ() const {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return chargeDirection_;
  }
  Math::Vector3 d = player->GetWorldPosition() - GetOwner()->GetWorldPosition();
  d.y = 0.0f;
  const float lenSq = d.LengthSquare();
  if (lenSq < 1e-6f) {
    return chargeDirection_;
  }
  return d * (1.0f / std::sqrt(lenSq));
}

bool CmpAtomFBoss::IsPlayerWithinXZ(const float radius) const {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return false;
  }
  Math::Vector3 d = player->GetWorldPosition() - GetOwner()->GetWorldPosition();
  d.y = 0.0f;
  const float reach = radius + AtomF::ResolvePlayerBodyRadiusMeters();
  return d.LengthSquare() <= reach * reach;
}

int CmpAtomFBoss::CountMinions() const {
  const std::vector<std::unique_ptr<Entity>> *entities =
      Script::SceneContext::Entities();
  if (entities == nullptr) {
    return 0;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  int count = 0;
  for (const std::unique_ptr<Entity> &e : *entities) {
    if (e == nullptr || !e->IsActive() || e.get() == GetOwner()) {
      continue;
    }
    if (tags.EntityHasRole(*e, "enemy") && !e->HasUpdCmp<CmpAtomFBoss>()) {
      ++count;
    }
  }
  return count;
}

float CmpAtomFBoss::SkillIntervalSeconds() const {
  return params_.skillIntervalSeconds *
         (enraged_ ? params_.enrageIntervalMultiplier : 1.0f);
}

Math::Vector3 CmpAtomFBoss::SummonSpawnPosition(const int index) const {
  const int n = std::max(1, params_.summonCount);
  const float angle = summonAngleOffsetRadians_ +
                      kTwoPi * static_cast<float>(index) / static_cast<float>(n);
  const Math::Vector3 center = GetOwner()->GetWorldPosition();
  Math::Vector3 p{center.x + std::cos(angle) * params_.summonRingRadiusMeters,
                  0.0f,
                  center.z + std::sin(angle) * params_.summonRingRadiusMeters};
  // アリーナの外に出ないよう内側へ寄せる。
  AtomF::ClampWorldTranslateToArenaDisc(p, AtomF::ResolveEnemyBodyRadiusMeters());
  return p;
}

void CmpAtomFBoss::EnterState(const State next) {
  state_ = next;
  stateElapsedSeconds_ = 0.0f;
  switch (next) {
  case State::Appearing:
    stateDurationSeconds_ = params_.appearSeconds;
    break;
  case State::Idle:
    stateDurationSeconds_ = SkillIntervalSeconds();
    break;
  case State::TelegraphCharge:
    stateDurationSeconds_ = params_.chargeTelegraphSeconds;
    chargeDirection_ = DirectionToPlayerXZ();
    break;
  case State::Charging:
    stateDurationSeconds_ = params_.chargeDurationSeconds;
    chargeHitPlayer_ = false;
    break;
  case State::TelegraphSummon:
    stateDurationSeconds_ = params_.summonTelegraphSeconds;
    summonAngleOffsetRadians_ = NextRandom01() * kTwoPi;
    break;
  case State::TelegraphNova:
    stateDurationSeconds_ = params_.novaTelegraphSeconds;
    break;
  case State::Stunned:
    stateDurationSeconds_ = params_.stunSeconds;
    sweatCooldownSeconds_ = 0.0f;
    break;
  case State::Dying:
    stateDurationSeconds_ = params_.deathSeconds;
    deathBurstCooldownSeconds_ = 0.0f;
    break;
  case State::Dormant:
    stateDurationSeconds_ = 1.0f;
    break;
  }
}

void CmpAtomFBoss::BeginAppear() {
  if (state_ != State::Dormant || GetOwner() == nullptr) {
    return;
  }
  Entity &owner = RefOwner();
  // 自機と反対側のアリーナ内に出現する。
  Math::Vector3 spawn{0.0f, 0.0f, 0.0f};
  if (Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
      player != nullptr && player->IsActive()) {
    Math::Vector3 away = player->GetWorldPosition();
    away.y = 0.0f;
    if (away.LengthSquare() > 1e-4f) {
      away = away * (-1.0f / std::sqrt(away.LengthSquare()));
      spawn = away * (AtomF::ResolveArenaInnerRadiusMeters() * 0.45f);
    } else {
      spawn = Math::Vector3{0.0f, 0.0f, AtomF::ResolveArenaInnerRadiusMeters() * 0.45f};
    }
  }
  spawn.y = BossCenterY(params_.bodyRadiusMeters);
  owner.RefTransform().translate = spawn;
  chargeDirection_ = DirectionToPlayerXZ();
  EnterState(State::Appearing);
  Particle::Combat::PlayBossDeathExplosionEffect(spawn, NextEffectSequence());
}

bool CmpAtomFBoss::TakeDamage(const int baseDamage) {
  if (!IsTargetable() || baseDamage <= 0 || GetOwner() == nullptr) {
    return false;
  }
  float damage = static_cast<float>(baseDamage);
  if (state_ == State::Stunned) {
    damage *= params_.stunDamageMultiplier;
  }
  health_ = std::max(0, health_ - static_cast<int>(std::lround(damage)));
  hurtFlashSeconds_ = 0.12f;
  Particle::Combat::PlayBossHitByReflectedBulletEffect(
      GetOwner()->GetWorldPosition());

  if (health_ <= 0) {
    BeginDying();
    return true;
  }
  if (!enraged_ &&
      static_cast<float>(health_) <=
          static_cast<float>(params_.maxHealth) * params_.enrageHealthRatio) {
    enraged_ = true; // 第 2 形態: 技の間隔が短くなる（耐久は上がらない）
    Particle::Combat::PlayParryReflectEffect(GetOwner()->GetWorldPosition());
    Particle::Combat::PlayBossDeathExplosionEffect(
        GetOwner()->GetWorldPosition(), NextEffectSequence());
  }
  return true;
}

void CmpAtomFBoss::BeginDying() {
  if (state_ == State::Dying) {
    return;
  }
  health_ = 0;
  EnterState(State::Dying);
  if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
    if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
      hit->SetIsActive(false);
    }
  }
}

void CmpAtomFBoss::CleanUpMinions() {
  const std::vector<std::unique_ptr<Entity>> *entities =
      Script::SceneContext::Entities();
  if (entities == nullptr) {
    return;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  for (const std::unique_ptr<Entity> &e : *entities) {
    if (e == nullptr || !e->IsActive() || e.get() == GetOwner()) {
      continue;
    }
    if (!tags.EntityHasRole(*e, "enemy") || e->HasUpdCmp<CmpAtomFBoss>()) {
      continue;
    }
    // ボス撃破の余波で残りの雑魚も消える（自機へのダメージなし）。
    Particle::Combat::PlayBossDeathExplosionEffect(e->GetWorldPosition(),
                                                   NextEffectSequence());
    e->Finalize();
    e->RequestDestroy();
  }
}

void CmpAtomFBoss::ChooseNextSkill() {
  const int cycle = skillCycle_++ % 3;
  if (cycle == 0) {
    EnterState(State::TelegraphCharge);
  } else if (cycle == 1 && CountMinions() < params_.maxMinions) {
    EnterState(State::TelegraphSummon);
  } else {
    EnterState(State::TelegraphNova);
  }
}

void CmpAtomFBoss::ExecuteSummon() {
  const int n = std::max(0, params_.summonCount);
  for (int i = 0; i < n; ++i) {
    const Math::Vector3 p = SummonSpawnPosition(i);
    AtomF::SpawnAtomFEnemyAt(p);
    Particle::Combat::PlayParryReflectEffect(
        Math::Vector3{p.x, AtomF::ResolveEnemyBodyRadiusMeters(), p.z});
  }
}

void CmpAtomFBoss::ExecuteNova() {
  const Math::Vector3 center = RefOwner().GetWorldPosition();
  Particle::Combat::PlayBossDeathExplosionEffect(center, NextEffectSequence());

  // 範囲内の雑魚を外へ吹き飛ばす（壁や他の雑魚にぶつかって連鎖爆発する）。
  if (const std::vector<std::unique_ptr<Entity>> *entities =
          Script::SceneContext::Entities()) {
    EntityTagRegistry &tags = EntityTagRegistry::Instance();
    tags.EnsureLoaded();
    const float reach = params_.novaRadiusMeters +
                        AtomF::ResolveEnemyBodyRadiusMeters();
    for (const std::unique_ptr<Entity> &e : *entities) {
      if (e == nullptr || !e->IsActive() || e.get() == GetOwner() ||
          e->HasUpdCmp<CmpAtomFBoss>() ||
          !tags.EntityHasRole(*e, "enemy")) {
        continue;
      }
      if (e->HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
        const CmpAtomFEnemyChainExplosion *chain =
            e->GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        if (chain != nullptr && chain->IsDying()) {
          continue;
        }
      }
      Math::Vector3 d = e->GetWorldPosition() - center;
      d.y = 0.0f;
      if (d.LengthSquare() > reach * reach) {
        continue;
      }
      if (CmpAtomFEnemyKnockback *knockback =
              e->GetUpdCmp<CmpAtomFEnemyKnockback>()) {
        knockback->ApplyKnockbackAwayFromExplosionCenter(
            center, params_.novaKnockbackSpeedMetersPerSecond);
      }
    }
  }

  if (IsPlayerWithinXZ(params_.novaRadiusMeters)) {
    if (Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity()) {
      if (CmpHealthHit *health = player->GetUpdCmp<CmpHealthHit>()) {
        health->TryTakeDamageUnconditional();
      }
    }
  }
}

void CmpAtomFBoss::UpdateAppearing(const float) {
  if (stateElapsedSeconds_ >= stateDurationSeconds_) {
    if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
      if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
        hit->SetIsActive(true);
      }
    }
    EnterState(State::Idle);
  }
}

void CmpAtomFBoss::UpdateIdle(const float dt) {
  const Math::Vector3 dir = DirectionToPlayerXZ();
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player != nullptr && player->IsActive()) {
    Math::Vector3 d = player->GetWorldPosition() - RefOwner().GetWorldPosition();
    d.y = 0.0f;
    const float stopDistance = params_.bodyRadiusMeters + 1.5f;
    if (d.LengthSquare() > stopDistance * stopDistance) {
      RefOwner().RefTransform().translate +=
          dir * (params_.moveSpeedMetersPerSecond * dt);
    }
  }
  if (stateElapsedSeconds_ >= stateDurationSeconds_) {
    ChooseNextSkill();
  }
}

void CmpAtomFBoss::UpdateTelegraphCharge(const float) {
  // 最後の 30% は向きを固定して、避ける猶予を作る。
  if (StateProgress() < 0.7f) {
    const Math::Vector3 want = DirectionToPlayerXZ();
    Math::Vector3 blended = chargeDirection_ * 0.9f + want * 0.1f;
    blended.y = 0.0f;
    if (blended.LengthSquare() > 1e-6f) {
      chargeDirection_ = blended * (1.0f / std::sqrt(blended.LengthSquare()));
    }
  }
  if (stateElapsedSeconds_ >= stateDurationSeconds_) {
    EnterState(State::Charging);
  }
}

void CmpAtomFBoss::UpdateCharging(const float dt) {
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate += chargeDirection_ * (params_.chargeSpeedMetersPerSecond * dt);

  if (!chargeHitPlayer_ && IsPlayerWithinXZ(params_.bodyRadiusMeters)) {
    chargeHitPlayer_ = true;
    if (Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity()) {
      if (CmpHealthHit *health = player->GetUpdCmp<CmpHealthHit>()) {
        health->TryTakeDamageUnconditional();
      }
    }
  }

  if (AtomF::IsWorldXZTouchingArenaWall(translate.x, translate.z,
                                        params_.bodyRadiusMeters)) {
    // 壁に激突: 目眩で隙を晒す。
    Particle::Combat::PlayBossDeathExplosionEffect(translate,
                                                   NextEffectSequence());
    EnterState(State::Stunned);
    return;
  }
  if (stateElapsedSeconds_ >= stateDurationSeconds_) {
    EnterState(State::Idle);
  }
}

void CmpAtomFBoss::UpdateTelegraphSummon(const float) {
  if (stateElapsedSeconds_ >= stateDurationSeconds_) {
    ExecuteSummon();
    EnterState(State::Idle);
  }
}

void CmpAtomFBoss::UpdateTelegraphNova(const float) {
  if (stateElapsedSeconds_ >= stateDurationSeconds_) {
    ExecuteNova();
    EnterState(State::Idle);
  }
}

void CmpAtomFBoss::UpdateStunned(const float dt) {
  sweatCooldownSeconds_ -= dt;
  if (sweatCooldownSeconds_ <= 0.0f) {
    sweatCooldownSeconds_ = 0.14f;
    const Math::Vector3 pos = RefOwner().GetWorldPosition();
    Particle::Combat::PlayEnemyPanicSweatEffect(
        Math::Vector3{pos.x, pos.y + params_.bodyRadiusMeters, pos.z},
        NextEffectSequence());
  }
  if (stateElapsedSeconds_ >= stateDurationSeconds_) {
    EnterState(State::Idle);
  }
}

void CmpAtomFBoss::UpdateDying(const float dt) {
  deathBurstCooldownSeconds_ -= dt;
  if (deathBurstCooldownSeconds_ <= 0.0f) {
    deathBurstCooldownSeconds_ = 0.22f;
    const Math::Vector3 pos = RefOwner().GetWorldPosition();
    const float angle = NextRandom01() * kTwoPi;
    const float r = NextRandom01() * params_.bodyRadiusMeters;
    Particle::Combat::PlayBossDeathExplosionEffect(
        Math::Vector3{pos.x + std::cos(angle) * r,
                      pos.y + (NextRandom01() - 0.3f) * params_.bodyRadiusMeters,
                      pos.z + std::sin(angle) * r},
        NextEffectSequence());
  }
  if (stateElapsedSeconds_ >= stateDurationSeconds_) {
    Particle::Combat::PlayBossDeathExplosionEffect(
        RefOwner().GetWorldPosition(), NextEffectSequence());
    CleanUpMinions();
    RefOwner().Finalize();
    RefOwner().RequestDestroy();
  }
}

void CmpAtomFBoss::ApplyVisual(const float dt) {
  if (!RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    return;
  }
  CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>();
  if (mesh == nullptr) {
    return;
  }
  hurtFlashSeconds_ = std::max(0.0f, hurtFlashSeconds_ - dt);

  const float R = params_.bodyRadiusMeters;
  const float p = StateProgress();
  const float t = stateElapsedSeconds_;
  float radius = R;
  float alpha = 1.0f;
  Math::Vector3 offset{};
  Math::Vector3 color{params_.colorR, params_.colorG, params_.colorB};

  auto mix = [&color](const Math::Vector3 &to, const float k) {
    color = color * (1.0f - k) + to * k;
  };
  const float pulse = 0.5f + 0.5f * std::sin(t * 24.0f);

  switch (state_) {
  case State::Dormant:
    radius = 0.01f;
    alpha = 0.0f;
    break;
  case State::Appearing: {
    radius = std::max(0.01f, R * EaseOutCubic(p));
    alpha = Saturate(p * 2.0f);
    const float amp = 0.15f * (1.0f - p);
    offset = {std::sin(t * 60.0f) * amp, 0.0f, std::cos(t * 53.0f) * amp};
    mix({0.9f, 0.9f, 1.0f}, 1.0f - p);
    break;
  }
  case State::TelegraphCharge: {
    const float amp = 0.04f + 0.1f * p;
    offset = {std::sin(t * 70.0f) * amp, 0.0f, std::cos(t * 61.0f) * amp};
    mix({1.0f, 0.15f, 0.1f}, 0.25f + 0.5f * pulse * p);
    break;
  }
  case State::TelegraphSummon: {
    radius = R * (1.0f + 0.08f * std::sin(p * kTwoPi * 2.0f));
    mix({0.2f, 0.9f, 1.0f}, 0.2f + 0.5f * p);
    break;
  }
  case State::TelegraphNova: {
    radius = R * (1.0f + 0.18f * p); // 膨らんで溜める
    const float amp = 0.03f + 0.08f * p;
    offset = {std::sin(t * 66.0f) * amp, 0.0f, std::cos(t * 59.0f) * amp};
    mix({1.0f, 0.55f, 0.1f}, 0.2f + 0.6f * pulse * p);
    break;
  }
  case State::Stunned:
    offset = {std::sin(t * 30.0f) * 0.12f, 0.0f, std::cos(t * 27.0f) * 0.12f};
    mix({0.45f, 0.6f, 1.0f}, 0.35f + 0.25f * pulse);
    break;
  case State::Dying: {
    const float amp = 0.1f + 0.25f * p;
    offset = {std::sin(t * 80.0f) * amp, 0.0f, std::cos(t * 71.0f) * amp};
    mix({1.0f, 0.4f, 0.2f}, 0.3f + 0.5f * pulse);
    if (p > 0.6f) {
      const float k = (p - 0.6f) / 0.4f;
      radius = R * (1.0f - k);
      alpha = 1.0f - k;
    }
    break;
  }
  default:
    break;
  }
  if (state_ == State::Charging) {
    mix({1.0f, 0.2f, 0.15f}, 0.5f);
  }
  if (hurtFlashSeconds_ > 0.0f) {
    mix({1.0f, 1.0f, 1.0f}, Saturate(hurtFlashSeconds_ / 0.12f) * 0.8f);
  }

  mesh->SetRadius(std::max(0.01f, radius));
  mesh->SetColor(Math::Vector4{color.x, color.y, color.z, alpha});
  mesh->SetDrawOffset(offset);
}

void CmpAtomFBoss::Update() {
  if (frameDeltaTime_ == nullptr || GetOwner() == nullptr) {
    return;
  }
  const float dt = frameDeltaTime_->GetDeltaTime();
  stateElapsedSeconds_ += dt;

  switch (state_) {
  case State::Dormant:
    break;
  case State::Appearing:
    UpdateAppearing(dt);
    break;
  case State::Idle:
    UpdateIdle(dt);
    break;
  case State::TelegraphCharge:
    UpdateTelegraphCharge(dt);
    break;
  case State::Charging:
    UpdateCharging(dt);
    break;
  case State::TelegraphSummon:
    UpdateTelegraphSummon(dt);
    break;
  case State::TelegraphNova:
    UpdateTelegraphNova(dt);
    break;
  case State::Stunned:
    UpdateStunned(dt);
    break;
  case State::Dying:
    UpdateDying(dt);
    break;
  }

  if (!RefOwner().IsActive()) {
    return; // 撃破演出を終えて破棄を依頼した
  }
  if (state_ != State::Dormant) {
    Math::Vector3 &translate = RefOwner().RefTransform().translate;
    translate.y = BossCenterY(params_.bodyRadiusMeters);
  }
  ApplyVisual(dt);
}

} // namespace Atrum
