#include "System/AtomF/CmpAtomFBoss.h"

#include "Math/Vector4.h"
#include "Scene/SceneBuildUpAtomFEnemySpawn.h"
#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Entity/HitJudge.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Atrum {

namespace {

constexpr float kTwoPi = 6.28318530718f;
constexpr float kPi = 3.14159265359f;
/// 召喚した雑魚の通し番号の開始値（初期配置の index と被らせない）。
constexpr int kSummonedEnemyIndexBase = 100;
/// 激昂の衝撃で雑魚を吹き飛ばす半径。
constexpr float kEnrageBlastRadiusMeters = 7.0f;

CmpAtomFBoss *g_activeBoss = nullptr;

float WrapPi(float angle) {
  while (angle > kPi) {
    angle -= kTwoPi;
  }
  while (angle < -kPi) {
    angle += kTwoPi;
  }
  return angle;
}

float Saturate(const float v) { return std::clamp(v, 0.0f, 1.0f); }

} // namespace

CmpAtomFBoss::~CmpAtomFBoss() {
  if (g_activeBoss == this) {
    g_activeBoss = nullptr;
  }
}

const CmpAtomFBoss *CmpAtomFBoss::Active() { return g_activeBoss; }

void CmpAtomFBoss::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  if (initialized_) {
    return; // 依存解決が再度呼ばれても Hp や状態を巻き戻さない。
  }
  initialized_ = true;
  params_ = AtomF::ResolveBossParameters();
  hp_ = params_.maxHp;
  g_activeBoss = this;
  SetState(State::Appearing);
}

void CmpAtomFBoss::Finalize() {
  if (g_activeBoss == this) {
    g_activeBoss = nullptr;
  }
}

bool CmpAtomFBoss::CanBeDamaged() const {
  return state_ != State::Appearing && state_ != State::Enraging &&
         state_ != State::Dying;
}

void CmpAtomFBoss::ApplyBulletHit() {
  if (!CanBeDamaged()) {
    return;
  }
  float amount = static_cast<float>(params_.bulletDamage);
  if (state_ == State::Stunned) {
    amount *= params_.stunDamageMultiplier;
  }
  Particle::Combat::PlayBossHitByReflectedBulletEffect(
      RefOwner().GetWorldPosition());
  ApplyDamage(static_cast<int>(std::lround(amount)));
}

void CmpAtomFBoss::ApplyExplosionHit() {
  if (!CanBeDamaged()) {
    return;
  }
  Particle::Combat::PlayBossHitByReflectedBulletEffect(
      RefOwner().GetWorldPosition());
  ApplyDamage(params_.explosionDamage);
}

void CmpAtomFBoss::ApplyDamage(const int amount) {
  hp_ = std::max(0, hp_ - amount);
  hitFlashRemaining_ = 0.12f;
  if (hp_ <= 0) {
    SetState(State::Dying);
    return;
  }
  if (!enraged_ &&
      static_cast<float>(hp_) <=
          static_cast<float>(params_.maxHp) * params_.enrageHpRatio) {
    SetState(State::Enraging);
  }
}

void CmpAtomFBoss::SetHitEnabled(const bool enabled) {
  if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
    hit->SetIsActive(enabled);
  }
}

bool CmpAtomFBoss::TryGetPlayerPosition(Math::Vector3 &out) const {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return false;
  }
  out = player->GetWorldPosition();
  return true;
}

void CmpAtomFBoss::EmitEffectAt(const char *fileName,
                                const Math::Vector3 &offset) {
  Particle::Combat::PlayOneShotEffectByFileName(
      fileName, RefOwner().GetWorldPosition() + offset);
}

int CmpAtomFBoss::ActiveSpokeCount() const {
  return enraged_ ? params_.shockwave.enragedSpokeCount
                  : params_.shockwave.spokeCount;
}

float CmpAtomFBoss::SpokeAngleRadians(const int spokeIndex) const {
  return spokeBaseAngle_ +
         kTwoPi * static_cast<float>(spokeIndex) /
             static_cast<float>(ActiveSpokeCount());
}

void CmpAtomFBoss::EmitSpokeEffects(const char *fileName,
                                    const float distanceMeters) {
  const int count = ActiveSpokeCount();
  for (int i = 0; i < count; ++i) {
    const float a = SpokeAngleRadians(i);
    EmitEffectAt(fileName, Math::Vector3{std::sin(a) * distanceMeters, 0.1f,
                                         std::cos(a) * distanceMeters});
  }
}

bool CmpAtomFBoss::IsPlayerInsideSpokes(const Math::Vector3 &playerPos,
                                        const float frontRadiusMeters) const {
  const Math::Vector3 self = RefOwner().GetWorldPosition();
  const float dx = playerPos.x - self.x;
  const float dz = playerPos.z - self.z;
  const float dist = std::sqrt(dx * dx + dz * dz);
  if (dist > frontRadiusMeters + params_.shockwave.bandHalfWidthMeters) {
    return false;
  }
  const float playerAngle = std::atan2(dx, dz);
  const float reach = params_.shockwave.spokeHalfWidthMeters +
                      AtomF::ResolvePlayerBodyRadiusMeters();
  const int count = ActiveSpokeCount();
  for (int i = 0; i < count; ++i) {
    const float delta = WrapPi(playerAngle - SpokeAngleRadians(i));
    if (std::cos(delta) <= 0.0f) {
      continue;
    }
    if (dist * std::abs(std::sin(delta)) <= reach) {
      return true;
    }
  }
  return false;
}

void CmpAtomFBoss::SpawnMinions() {
  const int count =
      enraged_ ? params_.summon.enragedCount : params_.summon.count;
  static int serial = kSummonedEnemyIndexBase;
  const Math::Vector3 self = RefOwner().GetWorldPosition();
  for (int i = 0; i < count; ++i) {
    const float a = kTwoPi * static_cast<float>(i) / static_cast<float>(count) +
                    spokeBaseAngle_;
    Math::Vector3 pos{self.x + std::sin(a) * params_.summon.spawnRingRadiusMeters,
                      0.0f,
                      self.z + std::cos(a) * params_.summon.spawnRingRadiusMeters};
    AtomF::ClampWorldTranslateToArenaDisc(
        pos, AtomF::ResolveEnemyBodyRadiusMeters());
    AtomF::EnqueueSceneEntityAdopt(CreateAtomFEnemy(pos, serial++));
    Particle::Combat::PlayOneShotEffectByFileName(
        "atomf_enemy_spawn.json", Math::Vector3{pos.x, 0.1f, pos.z});
  }
}

void CmpAtomFBoss::BlastAwayMinions(const float radiusMeters) {
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  const Math::Vector3 center = RefOwner().GetWorldPosition();
  const float speed = AtomF::ResolveEnemyKnockbackSpeedMetersPerSecond();

  for (CmpHitSphere *hitSphere : judge->RegisteredHitSpheres()) {
    if (hitSphere == nullptr || !hitSphere->GetIsActive()) {
      continue;
    }
    Entity *other = &hitSphere->RefOwner();
    if (other == &RefOwner() || !other->IsActive() ||
        !tags.EntityHasRole(*other, "enemy") ||
        tags.EntityHasRole(*other, "boss")) {
      continue;
    }
    if (const CmpAtomFEnemyChainExplosion *chain =
            other->GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      continue;
    }
    const Math::Vector3 d = other->GetWorldPosition() - center;
    if (d.x * d.x + d.z * d.z > radiusMeters * radiusMeters) {
      continue;
    }
    CmpAtomFEnemyKnockback *knockback =
        other->GetUpdCmp<CmpAtomFEnemyKnockback>();
    if (knockback == nullptr) {
      other->AddUpdCmp<CmpAtomFEnemyKnockback>();
      other->ResolveDependencies();
      knockback = other->GetUpdCmp<CmpAtomFEnemyKnockback>();
    }
    if (knockback != nullptr) {
      knockback->ApplyKnockbackAwayFromExplosionCenter(center, speed);
    }
  }
}

void CmpAtomFBoss::SetState(const State next) {
  state_ = next;
  stateElapsed_ = 0.0f;
  effectTimer_ = 0.0f;

  switch (next) {
  case State::Appearing:
    SetHitEnabled(false);
    visualScale_ = 0.0f;
    EmitEffectAt("atomf_boss_appear.json", Math::Vector3{0.0f, 0.0f, 0.0f});
    break;
  case State::Chasing:
    SetHitEnabled(true);
    break;
  case State::TelegraphSummon:
    EmitEffectAt("atomf_boss_telegraph.json", Math::Vector3{0.0f, 0.3f, 0.0f});
    break;
  case State::TelegraphShockwave:
    // 毎回違う向きの筋にする（決定論的な回転）。
    spokeBaseAngle_ = std::fmod(static_cast<float>(skillCycle_) * 2.399963f,
                                kTwoPi);
    shockwaveHitDone_ = false;
    EmitEffectAt("atomf_boss_telegraph.json", Math::Vector3{0.0f, 0.3f, 0.0f});
    for (float d = 1.5f; d <= params_.shockwave.maxRadiusMeters; d += 1.5f) {
      EmitSpokeEffects("atomf_boss_spoke_warn.json", d);
    }
    break;
  case State::Shockwave:
    shockwaveHitDone_ = false;
    EmitEffectAt("atomf_boss_shockwave.json", Math::Vector3{0.0f, 0.1f, 0.0f});
    break;
  case State::TelegraphDash:
    EmitEffectAt("atomf_boss_telegraph.json", Math::Vector3{0.0f, 0.3f, 0.0f});
    break;
  case State::Dashing: {
    Math::Vector3 playerPos{};
    const Math::Vector3 self = RefOwner().GetWorldPosition();
    if (TryGetPlayerPosition(playerPos)) {
      Math::Vector3 d{playerPos.x - self.x, 0.0f, playerPos.z - self.z};
      const float lenSq = d.LengthSquare();
      if (lenSq > 1e-6f) {
        dashDirection_ = d * (1.0f / std::sqrt(lenSq));
      }
    }
    break;
  }
  case State::Stunned:
    EmitEffectAt("atomf_boss_impact.json", Math::Vector3{0.0f, 0.3f, 0.0f});
    break;
  case State::Enraging:
    enraged_ = true;
    SetHitEnabled(false);
    EmitEffectAt("atomf_boss_enrage.json", Math::Vector3{0.0f, 0.2f, 0.0f});
    BlastAwayMinions(kEnrageBlastRadiusMeters);
    break;
  case State::Dying:
    SetHitEnabled(false);
    deathBurstCount_ = 0;
    break;
  }
}

void CmpAtomFBoss::UpdateAppearing(const float dt) {
  (void)dt;
  SetHitEnabled(false); // 初期化順に左右されないよう毎フレーム確認する。
  const float t = Saturate(stateElapsed_ / params_.appearSeconds);
  visualScale_ = 1.0f - (1.0f - t) * (1.0f - t);
  if (stateElapsed_ >= effectTimer_) {
    effectTimer_ += 0.4f;
    EmitEffectAt("atomf_boss_appear.json", Math::Vector3{0.0f, 0.0f, 0.0f});
  }
  if (stateElapsed_ >= params_.appearSeconds) {
    SetState(State::Chasing);
  }
}

void CmpAtomFBoss::UpdateChasing(const float dt) {
  Math::Vector3 playerPos{};
  if (TryGetPlayerPosition(playerPos)) {
    Math::Vector3 &translate = RefOwner().RefTransform().translate;
    Math::Vector3 d{playerPos.x - translate.x, 0.0f, playerPos.z - translate.z};
    const float lenSq = d.LengthSquare();
    const float keepDistance = params_.bodyRadiusMeters + 1.0f;
    if (lenSq > keepDistance * keepDistance) {
      float speed = params_.moveSpeedMetersPerSecond;
      if (enraged_) {
        speed *= params_.enrageSpeedMultiplier;
      }
      translate += d * (speed * dt / std::sqrt(lenSq));
    }
  }

  float interval = params_.skillIntervalSeconds;
  if (enraged_) {
    interval *= params_.enragedSkillIntervalMultiplier;
  }
  if (stateElapsed_ < interval) {
    return;
  }
  switch (skillCycle_ % 3) {
  case 0:
    SetState(State::TelegraphSummon);
    break;
  case 1:
    SetState(State::TelegraphShockwave);
    break;
  default:
    SetState(State::TelegraphDash);
    break;
  }
  ++skillCycle_;
}

void CmpAtomFBoss::UpdateTelegraphSummon(const float dt) {
  (void)dt;
  if (stateElapsed_ >= effectTimer_) {
    effectTimer_ += 0.3f;
    EmitEffectAt("atomf_boss_telegraph.json", Math::Vector3{0.0f, 0.3f, 0.0f});
  }
  if (stateElapsed_ >= params_.summon.telegraphSeconds) {
    SpawnMinions();
    SetState(State::Chasing);
  }
}

void CmpAtomFBoss::UpdateTelegraphShockwave(const float dt) {
  (void)dt;
  if (stateElapsed_ >= params_.shockwave.telegraphSeconds) {
    SetState(State::Shockwave);
  }
}

void CmpAtomFBoss::UpdateShockwave(const float dt) {
  (void)dt;
  const float t = Saturate(stateElapsed_ / params_.shockwave.expandSeconds);
  const float front = params_.shockwave.maxRadiusMeters * t;

  if (stateElapsed_ >= effectTimer_) {
    effectTimer_ += 0.15f;
    EmitSpokeEffects("atomf_boss_spoke_hit.json", front);
  }

  Math::Vector3 playerPos{};
  if (!shockwaveHitDone_ && TryGetPlayerPosition(playerPos) &&
      IsPlayerInsideSpokes(playerPos, front)) {
    shockwaveHitDone_ = true;
    Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
    if (player != nullptr) {
      if (CmpHealthHit *health = player->GetUpdCmp<CmpHealthHit>()) {
        health->TryTakeDamageUnconditional();
      }
    }
  }

  if (stateElapsed_ >= params_.shockwave.expandSeconds) {
    SetState(State::Chasing);
  }
}

void CmpAtomFBoss::UpdateTelegraphDash(const float dt) {
  (void)dt;
  if (stateElapsed_ >= effectTimer_) {
    effectTimer_ += 0.25f;
    EmitEffectAt("atomf_boss_telegraph.json", Math::Vector3{0.0f, 0.3f, 0.0f});
  }
  if (stateElapsed_ >= params_.dash.telegraphSeconds) {
    SetState(State::Dashing);
  }
}

void CmpAtomFBoss::UpdateDashing(const float dt) {
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  float speed = params_.dash.speedMetersPerSecond;
  if (enraged_) {
    speed *= params_.enrageSpeedMultiplier;
  }
  translate += dashDirection_ * (speed * dt);

  if (stateElapsed_ >= effectTimer_) {
    effectTimer_ += 0.08f;
    EmitEffectAt("atomf_boss_dash_dust.json", Math::Vector3{0.0f, 0.1f, 0.0f});
  }

  const bool hitWall = AtomF::IsWorldXZTouchingArenaWall(
      translate.x, translate.z, params_.bodyRadiusMeters);
  if (hitWall || stateElapsed_ >= params_.dash.durationSeconds) {
    SetState(State::Stunned);
  }
}

void CmpAtomFBoss::UpdateStunned(const float dt) {
  (void)dt;
  if (stateElapsed_ >= params_.dash.stunSeconds) {
    SetState(State::Chasing);
  }
}

void CmpAtomFBoss::UpdateEnraging(const float dt) {
  (void)dt;
  if (stateElapsed_ >= effectTimer_) {
    effectTimer_ += 0.5f;
    EmitEffectAt("atomf_boss_enrage.json", Math::Vector3{0.0f, 0.2f, 0.0f});
  }
  if (stateElapsed_ >= params_.enrageSeconds) {
    BlastAwayMinions(kEnrageBlastRadiusMeters);
    SetState(State::Chasing);
  }
}

void CmpAtomFBoss::UpdateDying(const float dt) {
  (void)dt;
  static uint32_t explosionSequence = 1000000U;
  if (stateElapsed_ >= effectTimer_ && stateElapsed_ < params_.deathSeconds) {
    effectTimer_ += 0.3f;
    // 体の周囲で連続して爆発させる。
    const float a = static_cast<float>(deathBurstCount_) * 2.399963f;
    const float r = params_.bodyRadiusMeters * 0.8f;
    ++deathBurstCount_;
    ++explosionSequence;
    Particle::Combat::PlayBossDeathExplosionEffect(
        RefOwner().GetWorldPosition() +
            Math::Vector3{std::sin(a) * r, 0.4f, std::cos(a) * r},
        explosionSequence);
  }
  if (stateElapsed_ >= params_.deathSeconds) {
    ++explosionSequence;
    Particle::Combat::PlayBossDeathExplosionEffect(
        RefOwner().GetWorldPosition() + Math::Vector3{0.0f, 0.5f, 0.0f},
        explosionSequence);
    EmitEffectAt("atomf_boss_death.json", Math::Vector3{0.0f, 0.5f, 0.0f});
    RefOwner().Finalize();
    RefOwner().RequestDestroy();
  }
}

void CmpAtomFBoss::ApplyVisuals(const float dt) {
  (void)dt;
  if (!RefOwner().HasDrawCmp<CmpMeshSphere>()) {
    return;
  }
  CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>();
  if (mesh == nullptr) {
    return;
  }

  float scale = visualScale_;
  Math::Vector3 shake{};
  float glow = 0.0f; // 白方向へ寄せる量
  float alpha = 1.0f;

  const float t = stateElapsed_;
  switch (state_) {
  case State::Appearing:
    glow = 1.0f - Saturate(t / params_.appearSeconds);
    break;
  case State::TelegraphSummon:
  case State::TelegraphShockwave:
  case State::TelegraphDash: {
    const float total = state_ == State::TelegraphSummon
                            ? params_.summon.telegraphSeconds
                        : state_ == State::TelegraphShockwave
                            ? params_.shockwave.telegraphSeconds
                            : params_.dash.telegraphSeconds;
    const float p = Saturate(t / total);
    const float amp = 0.05f + 0.12f * p;
    shake = Math::Vector3{std::sin(t * 70.0f) * amp, 0.0f,
                          std::cos(t * 83.0f) * amp};
    glow = 0.35f + 0.5f * std::abs(std::sin(t * 12.0f));
    scale = 1.0f + 0.08f * p; // 溜めで少し膨らむ
    break;
  }
  case State::Shockwave:
    scale = 1.0f - 0.1f * Saturate(t / 0.3f);
    break;
  case State::Stunned:
    shake = Math::Vector3{std::sin(t * 30.0f) * 0.04f, 0.0f, 0.0f};
    glow = 0.0f;
    break;
  case State::Enraging: {
    const float amp = 0.1f;
    shake = Math::Vector3{std::sin(t * 95.0f) * amp, 0.0f,
                          std::cos(t * 110.0f) * amp};
    glow = 0.5f + 0.5f * std::abs(std::sin(t * 18.0f));
    scale = 1.0f + 0.15f * std::abs(std::sin(t * 9.0f));
    break;
  }
  case State::Dying: {
    const float p = Saturate(t / params_.deathSeconds);
    const float amp = 0.05f + 0.2f * p;
    shake = Math::Vector3{std::sin(t * 120.0f) * amp, 0.0f,
                          std::cos(t * 97.0f) * amp};
    glow = 0.5f + 0.5f * std::abs(std::sin(t * 25.0f));
    const float fade = Saturate((t - (params_.deathSeconds - 0.8f)) / 0.8f);
    scale = 1.0f - fade;
    alpha = 1.0f - fade;
    break;
  }
  default:
    break;
  }

  if (hitFlashRemaining_ > 0.0f) {
    hitFlashRemaining_ = std::max(0.0f, hitFlashRemaining_ - dt);
    glow = std::max(glow, 0.9f);
  }

  Math::Vector4 color{params_.colorR, params_.colorG, params_.colorB, alpha};
  if (enraged_) {
    // 激昂後は赤みを足す。
    color.x = std::min(1.0f, color.x + 0.45f);
    color.z = std::max(0.0f, color.z - 0.25f);
  }
  color.x += (1.0f - color.x) * glow;
  color.y += (1.0f - color.y) * glow;
  color.z += (1.0f - color.z) * glow;

  mesh->SetRadius(params_.bodyRadiusMeters * scale);
  mesh->SetColor(color);
  mesh->SetDrawOffset(shake);
}

void CmpAtomFBoss::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  const float dt = frameDeltaTime_->GetDeltaTime();
  stateElapsed_ += dt;

  switch (state_) {
  case State::Appearing:
    UpdateAppearing(dt);
    break;
  case State::Chasing:
    UpdateChasing(dt);
    break;
  case State::TelegraphSummon:
    UpdateTelegraphSummon(dt);
    break;
  case State::TelegraphShockwave:
    UpdateTelegraphShockwave(dt);
    break;
  case State::Shockwave:
    UpdateShockwave(dt);
    break;
  case State::TelegraphDash:
    UpdateTelegraphDash(dt);
    break;
  case State::Dashing:
    UpdateDashing(dt);
    break;
  case State::Stunned:
    UpdateStunned(dt);
    break;
  case State::Enraging:
    UpdateEnraging(dt);
    break;
  case State::Dying:
    UpdateDying(dt);
    break;
  }

  ApplyVisuals(dt);
}

} // namespace Atrum
