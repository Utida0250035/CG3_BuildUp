#include "System/AtomF/CmpAtomFBoss.h"

#include "Math/Vector4.h"
#include "Scene/SceneBuildUpAtomFEnemySpawn.h"
#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
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
#include <string>

namespace Atrum {

namespace {

constexpr float kTwoPi = 6.28318530718f;

float Saturate(const float v) { return std::min(1.0f, std::max(0.0f, v)); }

float EaseOutCubic(const float t) {
  const float u = 1.0f - Saturate(t);
  return 1.0f - u * u * u;
}

Math::Vector4 LerpColor(const Math::Vector4 &a, const Math::Vector4 &b,
                        const float t) {
  return Math::Vector4{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
                       a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t};
}

/// 床（Y=0）上の位置。ボス本体の中心は半径分だけ浮いているので落とす。
CmpAtomFBoss *g_activeBoss = nullptr;

Math::Vector3 OnGround(const Math::Vector3 &p) {
  return Math::Vector3{p.x, 0.05f, p.z};
}

} // namespace

void CmpAtomFBoss::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  const AtomF::BossParameters &params = AtomF::ResolveBossParameters();
  maxHealth_ = params.maxHealth;
  health_ = params.maxHealth;
  bodyRadiusMeters_ = params.hitRadiusMeters;
  g_activeBoss = this;
}

void CmpAtomFBoss::Finalize() {
  if (g_activeBoss == this) {
    g_activeBoss = nullptr;
  }
}

CmpAtomFBoss *CmpAtomFBoss::Active() { return g_activeBoss; }

float CmpAtomFBoss::HealthBar01() const {
  if (state_ == State::Appear) {
    const float k = Saturate(stateElapsedSeconds_ /
                             AtomF::ResolveBossParameters().appearSeconds);
    return EaseOutCubic(k);
  }
  return Saturate(static_cast<float>(health_) /
                  static_cast<float>(std::max(1, maxHealth_)));
}

bool CmpAtomFBoss::HasTelegraph() const {
  return state_ == State::SummonTelegraph ||
         state_ == State::ShockwaveTelegraph || state_ == State::DashTelegraph;
}

float CmpAtomFBoss::TelegraphProgress01() const {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  switch (state_) {
  case State::SummonTelegraph:
    return Saturate(stateElapsedSeconds_ / p.summonTelegraphSeconds);
  case State::ShockwaveTelegraph:
    return Saturate(stateElapsedSeconds_ / p.shockwaveTelegraphSeconds);
  case State::DashTelegraph:
    return Saturate(stateElapsedSeconds_ / p.dashTelegraphSeconds);
  default:
    return 0.0f;
  }
}

float CmpAtomFBoss::ShockwaveRadiusMeters() const {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  return phase2_ ? p.shockwaveRadiusPhase2Meters
                 : p.shockwaveRadiusPhase1Meters;
}

Math::Vector3 CmpAtomFBoss::DirectionToPlayerXZ(float *outDistance) const {
  Math::Vector3 dir{0.0f, 0.0f, 1.0f};
  float dist = 0.0f;
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player != nullptr && player->IsActive()) {
    Math::Vector3 toPlayer =
        player->GetWorldPosition() - GetOwner()->GetWorldPosition();
    toPlayer.y = 0.0f;
    const float distSq = toPlayer.LengthSquare();
    if (distSq > 1e-6f) {
      dist = std::sqrt(distSq);
      dir = toPlayer * (1.0f / dist);
    }
  }
  if (outDistance != nullptr) {
    *outDistance = dist;
  }
  return dir;
}

float CmpAtomFBoss::ComputeDashLineLength(const Math::Vector3 &from,
                                          const Math::Vector3 &dirXZ) const {
  const float limit = AtomF::ResolveArenaInnerRadiusMeters() - bodyRadiusMeters_;
  const float b = from.x * dirXZ.x + from.z * dirXZ.z;
  const float c = from.x * from.x + from.z * from.z - limit * limit;
  const float disc = b * b - c;
  if (disc <= 0.0f) {
    return 0.0f;
  }
  return std::max(0.0f, -b + std::sqrt(disc));
}

bool CmpAtomFBoss::IsTouchingArenaWall() const {
  const Math::Vector3 p = GetOwner()->GetWorldPosition();
  return AtomF::IsWorldXZTouchingArenaWall(p.x, p.z, bodyRadiusMeters_);
}

void CmpAtomFBoss::EnterState(const State next) {
  state_ = next;
  stateElapsedSeconds_ = 0.0f;
  const AtomF::BossParameters &params = AtomF::ResolveBossParameters();
  Entity &owner = RefOwner();
  CmpHitSphere *hit = owner.GetUpdCmp<CmpHitSphere>();

  switch (next) {
  case State::Appear:
    if (hit != nullptr) {
      hit->SetIsActive(false);
    }
    Particle::Combat::PlayOneShotParticleFile(
        "./Assets/Particles/atomf_boss_appear.json",
        OnGround(owner.GetWorldPosition()));
    break;
  case State::Idle:
    if (hit != nullptr) {
      hit->SetIsActive(true);
    }
    idleTimerSeconds_ =
        phase2_ ? params.idleSecondsPhase2 : params.idleSecondsPhase1;
    break;
  case State::SummonTelegraph: {
    summonPoints_.clear();
    const int count =
        phase2_ ? params.summonCountPhase2 : params.summonCountPhase1;
    const Math::Vector3 center = owner.GetWorldPosition();
    const float limit = AtomF::ResolveArenaInnerRadiusMeters() - 1.0f;
    const float baseAngle = totalElapsedSeconds_ * 1.7f;
    for (int i = 0; i < count; ++i) {
      const float angle =
          baseAngle + kTwoPi * static_cast<float>(i) / static_cast<float>(count);
      Math::Vector3 p{center.x + std::cos(angle) * params.summonRingRadiusMeters,
                      0.0f,
                      center.z + std::sin(angle) * params.summonRingRadiusMeters};
      const float len = std::sqrt(p.x * p.x + p.z * p.z);
      if (len > limit && len > 1e-4f) {
        p.x *= limit / len;
        p.z *= limit / len;
      }
      summonPoints_.push_back(p);
    }
    break;
  }
  case State::ShockwaveTelegraph:
    break;
  case State::DashTelegraph:
    dashDirection_ = DirectionToPlayerXZ(nullptr);
    dashLineLengthMeters_ =
        ComputeDashLineLength(owner.GetWorldPosition(), dashDirection_);
    break;
  case State::Dashing:
    break;
  case State::DashRecover:
    break;
  case State::Stunned:
    stunSweatTimerSeconds_ = 0.0f;
    break;
  case State::Roar:
    roarPending_ = false;
    phase2_ = true;
    Particle::Combat::PlayOneShotParticleFile(
        "./Assets/Particles/atomf_boss_shockwave_p1.json",
        OnGround(owner.GetWorldPosition()));
    break;
  case State::Dying:
    break;
  }
  anchorTranslate_ = owner.RefTransform().translate;
}

bool CmpAtomFBoss::TakeDamage(const int baseDamage,
                              const Math::Vector3 &hitWorldPosition) {
  if (state_ == State::Appear || state_ == State::Roar ||
      state_ == State::Dying || health_ <= 0) {
    return false;
  }
  const AtomF::BossParameters &params = AtomF::ResolveBossParameters();
  const float multiplier =
      state_ == State::Stunned ? params.stunDamageMultiplier : 1.0f;
  const int damage = std::max(
      1, static_cast<int>(std::lround(static_cast<float>(baseDamage) *
                                      multiplier)));
  health_ = std::max(0, health_ - damage);
  hitFlashSeconds_ = params.hitFlashSeconds;
  Particle::Combat::PlayBossHitByReflectedBulletEffect(hitWorldPosition);

  if (health_ == 0) {
    BeginDying();
  } else if (!phase2_ && !roarPending_ &&
             static_cast<float>(health_) <=
                 static_cast<float>(maxHealth_) * params.phase2HealthRatio) {
    roarPending_ = true;
  }
  return true;
}

void CmpAtomFBoss::VanishAllMinions() {
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  for (CmpHitSphere *sphere : judge->RegisteredHitSpheres()) {
    if (sphere == nullptr || !sphere->GetIsActive()) {
      continue;
    }
    Entity *other = &sphere->RefOwner();
    if (other == &RefOwner() || !other->IsActive() ||
        !tags.EntityHasRole(*other, "enemy")) {
      continue;
    }
    CmpAtomFEnemyChainExplosion *chain =
        other->GetUpdCmp<CmpAtomFEnemyChainExplosion>();
    if (chain == nullptr || chain->IsDying()) {
      continue;
    }
    chain->BeginSilentVanish();
  }
}

void CmpAtomFBoss::BeginDying() {
  EnterState(State::Dying);
  dyingExplosionTimerSeconds_ = 0.0f;
  if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
    hit->SetIsActive(false);
  }
  VanishAllMinions();
}

void CmpAtomFBoss::DamagePlayerIfInside(const float radiusMeters) {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return;
  }
  CmpHealthHit *health = player->GetUpdCmp<CmpHealthHit>();
  if (health == nullptr) {
    return;
  }
  Math::Vector3 diff = player->GetWorldPosition() - RefOwner().GetWorldPosition();
  diff.y = 0.0f;
  const float reach = radiusMeters + AtomF::ResolvePlayerBodyRadiusMeters();
  if (diff.LengthSquare() <= reach * reach) {
    health->TryTakeDamageUnconditional();
  }
}

void CmpAtomFBoss::ChooseNextSkill() {
  static const Skill kPhase1[] = {Skill::Summon, Skill::Shockwave, Skill::Dash};
  static const Skill kPhase2[] = {Skill::Shockwave, Skill::Dash, Skill::Summon,
                                  Skill::Dash};
  if (phase2_) {
    currentSkill_ = kPhase2[skillCursor_ % 4];
  } else {
    currentSkill_ = kPhase1[skillCursor_ % 3];
  }
  ++skillCursor_;
  switch (currentSkill_) {
  case Skill::Summon:
    EnterState(State::SummonTelegraph);
    break;
  case Skill::Shockwave:
    EnterState(State::ShockwaveTelegraph);
    break;
  case Skill::Dash:
    EnterState(State::DashTelegraph);
    break;
  }
}

void CmpAtomFBoss::ExecuteSummon() {
  const auto &variants = AtomF::ParametersRuntime::Instance().Enemy().variants;
  for (const Math::Vector3 &point : summonPoints_) {
    std::string name = "walker";
    if (!variants.empty()) {
      name = variants[static_cast<std::size_t>(summonSerial_) % variants.size()]
                 .name;
    }
    ++summonSerial_;
    EnqueueAtomFEnemy(name, point.x, point.z);
    Particle::Combat::PlayOneShotParticleFile(
        "./Assets/Particles/atomf_boss_summon.json", OnGround(point));
  }
  summonPoints_.clear();
}

void CmpAtomFBoss::ExecuteShockwave() {
  const float radius = ShockwaveRadiusMeters();
  Particle::Combat::PlayOneShotParticleFile(
      phase2_ ? "./Assets/Particles/atomf_boss_shockwave_p2.json"
              : "./Assets/Particles/atomf_boss_shockwave_p1.json",
      OnGround(RefOwner().GetWorldPosition()));
  DamagePlayerIfInside(radius);
}

void CmpAtomFBoss::UpdateAppear(const float) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  if (stateElapsedSeconds_ >= p.appearSeconds) {
    EnterState(State::Idle);
  }
}

void CmpAtomFBoss::UpdateIdle(const float dt) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();

  if (roarPending_) {
    EnterState(State::Roar);
    return;
  }

  float dist = 0.0f;
  const Math::Vector3 dir = DirectionToPlayerXZ(&dist);
  if (dist > bodyRadiusMeters_ + 1.5f) {
    const float speed = p.moveSpeedMetersPerSecond * (phase2_ ? 1.5f : 1.0f);
    RefOwner().RefTransform().translate += dir * (speed * dt);
  }

  idleTimerSeconds_ -= dt;
  if (idleTimerSeconds_ <= 0.0f) {
    ChooseNextSkill();
  }
}

void CmpAtomFBoss::UpdateSummonTelegraph(const float) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  if (stateElapsedSeconds_ >= p.summonTelegraphSeconds) {
    ExecuteSummon();
    EnterState(State::Idle);
    idleTimerSeconds_ = 0.6f;
  }
}

void CmpAtomFBoss::UpdateShockwaveTelegraph(const float) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  if (stateElapsedSeconds_ >= p.shockwaveTelegraphSeconds) {
    ExecuteShockwave();
    EnterState(State::Idle);
    idleTimerSeconds_ = 0.8f;
  }
}

void CmpAtomFBoss::UpdateDashTelegraph(const float) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  // 発動直前の 30% は向きを固定して、避ける猶予を与える。
  if (TelegraphProgress01() < 0.7f) {
    dashDirection_ = DirectionToPlayerXZ(nullptr);
  }
  dashLineLengthMeters_ =
      ComputeDashLineLength(anchorTranslate_, dashDirection_);
  RefOwner().RefTransform().translate = anchorTranslate_;
  if (stateElapsedSeconds_ >= p.dashTelegraphSeconds) {
    EnterState(State::Dashing);
  }
}

void CmpAtomFBoss::UpdateDashing(const float dt) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  RefOwner().RefTransform().translate +=
      dashDirection_ * (p.dashSpeedMetersPerSecond * dt);

  // 壁際から内側へ走り出した直後は激突扱いにしない（外向きに動いているときだけ）。
  const Math::Vector3 pos = RefOwner().GetWorldPosition();
  const bool movingOutward =
      pos.x * dashDirection_.x + pos.z * dashDirection_.z > 0.0f;
  if (movingOutward && IsTouchingArenaWall()) {
    Particle::Combat::PlayOneShotParticleFile(
        "./Assets/Particles/atomf_boss_wallslam.json",
        RefOwner().GetWorldPosition());
    EnterState(State::Stunned);
    return;
  }
  if (stateElapsedSeconds_ >= p.dashMaxSeconds) {
    EnterState(State::DashRecover);
  }
}

void CmpAtomFBoss::UpdateDashRecover(const float) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  if (stateElapsedSeconds_ >= p.dashRecoverSeconds) {
    EnterState(State::Idle);
    idleTimerSeconds_ = 0.6f;
  }
}

void CmpAtomFBoss::UpdateStunned(const float dt) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  stunSweatTimerSeconds_ -= dt;
  if (stunSweatTimerSeconds_ <= 0.0f) {
    stunSweatTimerSeconds_ = 0.3f;
    static uint32_t sweatSequence = 0U;
    ++sweatSequence;
    Particle::Combat::PlayEnemyPanicSweatEffect(
        RefOwner().GetWorldPosition() +
            Math::Vector3{0.0f, bodyRadiusMeters_, 0.0f},
        100000U + sweatSequence);
  }
  if (stateElapsedSeconds_ >= p.stunSeconds) {
    EnterState(State::Idle);
    idleTimerSeconds_ = 0.8f;
  }
}

void CmpAtomFBoss::UpdateRoar(const float) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  if (stateElapsedSeconds_ >= p.roarSeconds) {
    EnterState(State::Idle);
    idleTimerSeconds_ = 0.5f;
  }
}

void CmpAtomFBoss::UpdateDying(const float dt) {
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();
  dyingExplosionTimerSeconds_ -= dt;
  if (dyingExplosionTimerSeconds_ <= 0.0f) {
    dyingExplosionTimerSeconds_ = 0.22f;
    static uint32_t sequence = 500000U;
    ++sequence;
    const float a = totalElapsedSeconds_ * 9.0f;
    const Math::Vector3 center = RefOwner().GetWorldPosition();
    Particle::Combat::PlayBossDeathExplosionEffect(
        center + Math::Vector3{std::cos(a) * bodyRadiusMeters_ * 0.7f,
                               std::sin(a * 1.7f) * bodyRadiusMeters_ * 0.5f,
                               std::sin(a) * bodyRadiusMeters_ * 0.7f},
        sequence);
  }
  if (stateElapsedSeconds_ >= p.deathSeconds) {
    static uint32_t finalSequence = 900000U;
    Particle::Combat::PlayBossDeathExplosionEffect(
        RefOwner().GetWorldPosition(), ++finalSequence);
    RefOwner().Finalize();
    RefOwner().RequestDestroy();
  }
}

void CmpAtomFBoss::ApplyVisual(const float) {
  CmpMeshSphere *mesh = RefOwner().GetDrawCmp<CmpMeshSphere>();
  if (mesh == nullptr) {
    return;
  }
  const AtomF::BossParameters &p = AtomF::ResolveBossParameters();

  Math::Vector4 base = phase2_ ? Math::Vector4{0.95f, 0.18f, 0.2f, 1.0f}
                               : Math::Vector4{0.6f, 0.22f, 0.9f, 1.0f};
  float scale = 1.0f;
  Math::Vector3 offset{0.0f, 0.0f, 0.0f};
  const float t = totalElapsedSeconds_;

  switch (state_) {
  case State::Appear: {
    const float k = Saturate(stateElapsedSeconds_ / p.appearSeconds);
    scale = EaseOutCubic(k);
    base.w = k;
    break;
  }
  case State::SummonTelegraph:
  case State::ShockwaveTelegraph:
  case State::DashTelegraph: {
    const float k = TelegraphProgress01();
    base = LerpColor(base, Math::Vector4{1.0f, 0.95f, 0.5f, 1.0f},
                     0.5f * k * (0.5f + 0.5f * std::sin(t * 30.0f)));
    scale = 1.0f + 0.06f * k * std::sin(t * 40.0f);
    if (state_ == State::DashTelegraph) {
      offset = Math::Vector3{std::sin(t * 70.0f) * 0.12f * k, 0.0f,
                             std::cos(t * 63.0f) * 0.12f * k};
    }
    break;
  }
  case State::Stunned:
    base = Math::Vector4{0.35f, 0.75f, 1.0f, 1.0f};
    offset = Math::Vector3{std::sin(t * 6.0f) * 0.15f, 0.0f,
                           std::cos(t * 5.0f) * 0.15f};
    break;
  case State::Roar: {
    const float k = Saturate(stateElapsedSeconds_ / p.roarSeconds);
    scale = 1.0f + 0.12f * std::sin(t * 36.0f) * (1.0f - k * 0.5f);
    offset = Math::Vector3{std::sin(t * 80.0f) * 0.1f, 0.0f,
                           std::cos(t * 71.0f) * 0.1f};
    base = LerpColor(Math::Vector4{0.6f, 0.22f, 0.9f, 1.0f},
                     Math::Vector4{0.95f, 0.18f, 0.2f, 1.0f}, k);
    break;
  }
  case State::Dying: {
    const float k = Saturate(stateElapsedSeconds_ / p.deathSeconds);
    scale = 1.0f - EaseOutCubic(std::max(0.0f, (k - 0.5f) * 2.0f));
    base.w = scale;
    offset = Math::Vector3{std::sin(t * 90.0f) * 0.2f * k, 0.0f,
                           std::cos(t * 83.0f) * 0.2f * k};
    break;
  }
  default:
    break;
  }

  if (hitFlashSeconds_ > 0.0f && p.hitFlashSeconds > 0.0f) {
    const float k = Saturate(hitFlashSeconds_ / p.hitFlashSeconds);
    const float alpha = base.w;
    base = LerpColor(base, Math::Vector4{1.0f, 1.0f, 1.0f, alpha}, 0.7f * k);
  }

  mesh->SetRadius(bodyRadiusMeters_ * scale);
  mesh->SetColor(base);
  mesh->SetDrawOffset(offset);
}

void CmpAtomFBoss::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  const float dt = frameDeltaTime_->GetDeltaTime();
  if (!started_) {
    started_ = true;
    EnterState(State::Appear);
  }
  totalElapsedSeconds_ += dt;
  stateElapsedSeconds_ += dt;
  hitFlashSeconds_ = std::max(0.0f, hitFlashSeconds_ - dt);

  switch (state_) {
  case State::Appear:
    UpdateAppear(dt);
    break;
  case State::Idle:
    UpdateIdle(dt);
    break;
  case State::SummonTelegraph:
    UpdateSummonTelegraph(dt);
    break;
  case State::ShockwaveTelegraph:
    UpdateShockwaveTelegraph(dt);
    break;
  case State::DashTelegraph:
    UpdateDashTelegraph(dt);
    break;
  case State::Dashing:
    UpdateDashing(dt);
    break;
  case State::DashRecover:
    UpdateDashRecover(dt);
    break;
  case State::Stunned:
    UpdateStunned(dt);
    break;
  case State::Roar:
    UpdateRoar(dt);
    break;
  case State::Dying:
    UpdateDying(dt);
    if (!RefOwner().IsActive()) {
      return;
    }
    break;
  }
  ApplyVisual(dt);
}

} // namespace Atrum
