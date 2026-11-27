#include "System/AtomF/CmpAtomFBoss.h"

#include "Camera/CombatCameraShake.h"
#include "Math/Vector4.h"
#include "Scene/SceneBuildUpAtomFEnemySpawn.h"
#include "System/AtomF/AtomFEnemyExplosionKnockbackSpread.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Components/CmpMeshSphere.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"
#include "System/PlaySe.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

namespace {

constexpr float kPi = 3.14159265359f;
constexpr float kTwoPi = 6.28318530718f;

constexpr const char *kFxTelegraph = "./Assets/Particles/boss_telegraph.json";
constexpr const char *kFxRingWarning =
    "./Assets/Particles/boss_ring_warning.json";
constexpr const char *kFxShockwave = "./Assets/Particles/boss_shockwave.json";
constexpr const char *kFxSpawn = "./Assets/Particles/boss_spawn.json";
constexpr const char *kFxDashTrail = "./Assets/Particles/boss_dash_trail.json";
constexpr const char *kFxSummon = "./Assets/Particles/enemy_spawn.json";

constexpr float kRecoverySeconds = 0.7f;
constexpr float kRoarSeconds = 1.6f;
constexpr float kDyingExplosionIntervalSeconds = 0.22f;

float EaseOutCubic(const float t) {
  const float u = 1.0f - std::clamp(t, 0.0f, 1.0f);
  return 1.0f - u * u * u;
}

float Lerp(const float a, const float b, const float t) {
  return a + (b - a) * t;
}

} // namespace

void CmpAtomFBoss::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  params_ = AtomF::ResolveBossParameters();
  health_ = RefOwner().GetUpdCmp<CmpHealthHit>();
  hit_ = RefOwner().GetUpdCmp<CmpHitSphere>();
  mesh_ = RefOwner().GetDrawCmp<CmpMeshSphere>();
}

void CmpAtomFBoss::Initialize() {
  state_ = State::Appearing;
  stateElapsedSeconds_ = 0.0f;
  skillTimerSeconds_ = 0.0f;
  enraged_ = false;
  if (health_ != nullptr) {
    health_->SetInitialHealthCount(static_cast<uint32_t>(params_.maxHealth));
    health_->SetHealthCount(static_cast<uint32_t>(params_.maxHealth));
  }
  SetHitActive(false);
  const Math::Vector3 pos = RefOwner().GetWorldPosition();
  Particle::Combat::PlayOneShotParticleFile(
      kFxSpawn, Math::Vector3{pos.x, 0.05f, pos.z});
}

int CmpAtomFBoss::GetHealth() const {
  return health_ != nullptr ? static_cast<int>(health_->GetHealthCount()) : 0;
}

void CmpAtomFBoss::SetHitActive(const bool active) {
  if (hit_ != nullptr) {
    hit_->SetIsActive(active);
  }
}

float CmpAtomFBoss::NextRandom01() {
  rngState_ = rngState_ * 1664525u + 1013904223u;
  return static_cast<float>((rngState_ >> 8) & 0xFFFFu) / 65535.0f;
}

void CmpAtomFBoss::ApplyVisual(const float radiusScale,
                               const Math::Vector3 &shakeOffset, const float r,
                               const float g, const float b,
                               const float whiteFlash) {
  if (mesh_ == nullptr) {
    return;
  }
  mesh_->SetRadius(params_.bodyRadiusMeters * std::max(0.0f, radiusScale));
  mesh_->SetDrawOffset(shakeOffset);
  const float f = std::clamp(whiteFlash, 0.0f, 1.0f);
  mesh_->SetColor(Math::Vector4{Lerp(r, 1.0f, f), Lerp(g, 1.0f, f),
                                Lerp(b, 1.0f, f), 1.0f});
}

Math::Vector3 CmpAtomFBoss::DirectionToPlayerXZ() const {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return Math::Vector3{0.0f, 0.0f, -1.0f};
  }
  const Entity &self = *GetOwner();
  Math::Vector3 d = player->GetWorldPosition() - self.GetWorldPosition();
  d.y = 0.0f;
  const float lenSq = d.LengthSquare();
  if (lenSq < 1e-6f) {
    return Math::Vector3{0.0f, 0.0f, -1.0f};
  }
  return d * (1.0f / std::sqrt(lenSq));
}

float CmpAtomFBoss::CurrentSkillInterval() const {
  return enraged_ ? params_.enragedSkillIntervalSeconds
                  : params_.skillIntervalSeconds;
}

float CmpAtomFBoss::TelegraphSecondsFor(const Skill skill) const {
  switch (skill) {
  case Skill::RingBlast:
    return params_.ringBlastTelegraphSeconds;
  case Skill::Dash:
    return params_.dashTelegraphSeconds;
  case Skill::Summon:
    return params_.summonTelegraphSeconds;
  }
  return 1.0f;
}

CmpAtomFBoss::Skill CmpAtomFBoss::PickNextSkill() {
  // 輪衝撃 → 召喚 → 突進 の順に巡回（雑魚を呼んで玉をぶつける機会を作る）。
  static constexpr Skill kCycle[3] = {Skill::RingBlast, Skill::Summon,
                                      Skill::Dash};
  const Skill skill = kCycle[skillCycleIndex_ % 3];
  ++skillCycleIndex_;
  return skill;
}

bool CmpAtomFBoss::TakeDamage(const int amount) {
  if (health_ == nullptr || amount <= 0 || state_ == State::Appearing ||
      state_ == State::Dying) {
    return false;
  }

  float scaled = static_cast<float>(amount);
  if (state_ == State::Staggered) {
    scaled *= params_.staggerDamageMultiplier;
  }
  const uint32_t damage = static_cast<uint32_t>(std::ceil(scaled));
  const uint32_t current = health_->GetHealthCount();
  const uint32_t next = damage >= current ? 0U : current - damage;
  health_->SetHealthCount(next);

  hitFlashSeconds_ = 0.14f;
  CombatCameraShake::Instance().RequestBossHitShake();
  PlaySeCandidates({"./Assets/Audios/seHit.mp3", "./Resources/seHit.mp3"});
  Particle::Combat::PlayBossHitByReflectedBulletEffect(
      RefOwner().GetWorldPosition());

  if (next == 0U) {
    BeginDying();
    return true;
  }

  staggerAccumulatedDamage_ += scaled;
  if (state_ != State::Staggered && state_ != State::Roaring &&
      staggerCooldownRemainingSeconds_ <= 0.0f &&
      staggerAccumulatedDamage_ >= params_.staggerDamageThreshold) {
    BeginStagger();
  }
  return true;
}

void CmpAtomFBoss::BeginTelegraph(const Skill skill) {
  currentSkill_ = skill;
  state_ = State::Telegraph;
  stateElapsedSeconds_ = 0.0f;
  effectTimerSeconds_ = 0.0f;
  if (skill == Skill::Dash) {
    dashDirection_ = DirectionToPlayerXZ();
  }
}

void CmpAtomFBoss::BeginExecuting() {
  state_ = State::Executing;
  stateElapsedSeconds_ = 0.0f;
  effectTimerSeconds_ = 0.0f;
  executeActionDone_ = false;
  executeDurationSeconds_ = currentSkill_ == Skill::Dash
                                ? params_.dashDurationSeconds
                                : kRecoverySeconds;
  if (currentSkill_ == Skill::Dash) {
    // 突進の向きは予備動作の終わりに自機方向へ確定する。
    dashDirection_ = DirectionToPlayerXZ();
    CombatCameraShake::Instance().RequestBossHitShake();
  }
}

void CmpAtomFBoss::BeginStagger() {
  state_ = State::Staggered;
  stateElapsedSeconds_ = 0.0f;
  effectTimerSeconds_ = 0.0f;
  staggerAccumulatedDamage_ = 0.0f;
  staggerCooldownRemainingSeconds_ = params_.staggerCooldownSeconds;
  CombatCameraShake::Instance().RequestBossHitShake();
}

void CmpAtomFBoss::BeginRoar() {
  state_ = State::Roaring;
  stateElapsedSeconds_ = 0.0f;
  effectTimerSeconds_ = 0.0f;
  enraged_ = true;
}

void CmpAtomFBoss::BeginDying() {
  state_ = State::Dying;
  stateElapsedSeconds_ = 0.0f;
  dyingExplosionTimerSeconds_ = 0.0f;
  SetHitActive(false);
}

void CmpAtomFBoss::FinishSkillToWalking() {
  state_ = State::Walking;
  stateElapsedSeconds_ = 0.0f;
  skillTimerSeconds_ = 0.0f;
}

void CmpAtomFBoss::ExecuteRingBlast() {
  const Math::Vector3 center = RefOwner().GetWorldPosition();
  Particle::Combat::PlayOneShotParticleFile(
      kFxShockwave, Math::Vector3{center.x, 0.1f, center.z});
  CombatCameraShake::Instance().RequestPlayerHitShake();
  PlaySeCandidates({"./Assets/Audios/seHit.mp3", "./Resources/seHit.mp3"});

  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return;
  }
  CmpHitSphere *playerHit = player->GetUpdCmp<CmpHitSphere>();
  CmpHealthHit *playerHealth = player->GetUpdCmp<CmpHealthHit>();
  if (playerHit == nullptr || playerHealth == nullptr ||
      !playerHit->GetIsActive()) {
    return;
  }
  const Math::Vector3 flatCenter{center.x, player->GetWorldPosition().y,
                                 center.z};
  if (AtomF::IsExplosionSphereIntersectingTarget(
          flatCenter, params_.ringBlastRadiusMeters, *player, *playerHit)) {
    playerHealth->TryTakeDamageUnconditional();
  }
}

void CmpAtomFBoss::ExecuteSummon() {
  const int variantCount = AtomF::EnemyVariantCount();
  if (variantCount <= 0) {
    return;
  }
  const int count = params_.summonCount +
                    (enraged_ ? params_.enragedSummonBonus : 0);
  const Math::Vector3 center = RefOwner().GetWorldPosition();
  const float startAngle = NextRandom01() * kTwoPi;
  for (int i = 0; i < count; ++i) {
    const float angle =
        startAngle + kTwoPi * static_cast<float>(i) / static_cast<float>(count);
    const float radius =
        params_.bodyRadiusMeters + params_.summonRadiusMeters;
    const int variant = static_cast<int>(NextRandom01() * 100.0f) %
                        variantCount;
    AtomF::SpawnEnemyAtWorld(variant, center.x + std::cos(angle) * radius,
                             center.z + std::sin(angle) * radius);
  }
  Particle::Combat::PlayOneShotParticleFile(
      kFxSummon, Math::Vector3{center.x, 0.1f, center.z});
  CombatCameraShake::Instance().RequestBossHitShake();
}

void CmpAtomFBoss::UpdateAppearing(const float dt) {
  stateElapsedSeconds_ += dt;
  const float t = stateElapsedSeconds_ / params_.appearSeconds;
  const float e = EaseOutCubic(t);

  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate.y = Lerp(-params_.bodyRadiusMeters * 1.2f,
                     params_.bodyRadiusMeters, e);

  // 地面から湧き上がる粒を一定間隔で出し、地鳴りのシェイクを入れる。
  effectTimerSeconds_ += dt;
  if (effectTimerSeconds_ >= 0.25f) {
    effectTimerSeconds_ = 0.0f;
    Particle::Combat::PlayOneShotParticleFile(
        kFxSpawn, Math::Vector3{translate.x, 0.05f, translate.z});
  }
  shakeTimerSeconds_ += dt;
  if (shakeTimerSeconds_ >= 0.5f) {
    shakeTimerSeconds_ = 0.0f;
    CombatCameraShake::Instance().RequestBossHitShake();
  }

  const float flash = 0.5f * (1.0f - e);
  ApplyVisual(Lerp(0.4f, 1.0f, e), Math::Vector3{}, params_.colorR,
              params_.colorG, params_.colorB, flash);

  if (stateElapsedSeconds_ >= params_.appearSeconds) {
    translate.y = params_.bodyRadiusMeters;
    SetHitActive(true);
    Particle::Combat::PlayOneShotParticleFile(
        kFxShockwave, Math::Vector3{translate.x, 0.1f, translate.z});
    CombatCameraShake::Instance().RequestPlayerHitShake();
    FinishSkillToWalking();
  }
}

void CmpAtomFBoss::UpdateWalking(const float dt) {
  const float ratio = static_cast<float>(GetHealth()) /
                      static_cast<float>(std::max(1, params_.maxHealth));
  if (!enraged_ && ratio <= params_.enrageHealthRatio) {
    BeginRoar();
    return;
  }

  const Math::Vector3 dir = DirectionToPlayerXZ();
  RefOwner().RefTransform().translate +=
      dir * (params_.walkSpeedMetersPerSecond * (enraged_ ? 1.5f : 1.0f) * dt);

  skillTimerSeconds_ += dt;
  if (skillTimerSeconds_ >= CurrentSkillInterval()) {
    BeginTelegraph(PickNextSkill());
    return;
  }

  const float baseR = enraged_ ? 0.95f : params_.colorR;
  const float baseG = enraged_ ? 0.15f : params_.colorG;
  const float baseB = enraged_ ? 0.2f : params_.colorB;
  ApplyVisual(1.0f, Math::Vector3{}, baseR, baseG, baseB,
              hitFlashSeconds_ > 0.0f ? 0.8f : 0.0f);
}

void CmpAtomFBoss::UpdateTelegraph(const float dt) {
  stateElapsedSeconds_ += dt;
  const float total = TelegraphSecondsFor(currentSkill_);
  const float t = std::clamp(stateElapsedSeconds_ / total, 0.0f, 1.0f);

  // 予備動作: 溜めるほど震えが強くなり、点滅が速くなる。
  const float shakeAmp = 0.04f + 0.12f * t;
  const float phase = stateElapsedSeconds_ * (30.0f + 40.0f * t);
  const Math::Vector3 shake{std::sin(phase) * shakeAmp, 0.0f,
                            std::cos(phase * 1.3f) * shakeAmp * 0.7f};
  const float blink =
      0.5f + 0.5f * std::sin(stateElapsedSeconds_ * (10.0f + 14.0f * t));
  const float whiteFlash = 0.15f + 0.6f * t * blink;
  // スキルごとに溜め色を変えて、見分けられるようにする。
  float r = 1.0f;
  float g = 0.25f;
  float b = 0.2f; // 輪衝撃: 赤
  if (currentSkill_ == Skill::Dash) {
    r = 1.0f;
    g = 0.75f;
    b = 0.1f; // 突進: 黄
  } else if (currentSkill_ == Skill::Summon) {
    r = 0.2f;
    g = 0.85f;
    b = 0.9f; // 召喚: 水色
  }
  ApplyVisual(1.0f - 0.08f * t, shake, r, g, b, whiteFlash);

  effectTimerSeconds_ += dt;
  if (effectTimerSeconds_ >= 0.2f) {
    effectTimerSeconds_ = 0.0f;
    const Math::Vector3 pos = RefOwner().GetWorldPosition();
    Particle::Combat::PlayOneShotParticleFile(kFxTelegraph, pos);
    if (currentSkill_ == Skill::RingBlast) {
      // 危険範囲を地面の粒で示す。
      Particle::Combat::PlayOneShotParticleFile(
          kFxRingWarning, Math::Vector3{pos.x, 0.05f, pos.z});
    }
  }

  if (currentSkill_ == Skill::Dash) {
    // 溜めの前半は自機を追って向きを更新、後半で固定する（避ける猶予を作る）。
    if (t < 0.6f) {
      dashDirection_ = DirectionToPlayerXZ();
    }
  }

  if (stateElapsedSeconds_ >= total) {
    BeginExecuting();
  }
}

void CmpAtomFBoss::UpdateExecuting(const float dt) {
  stateElapsedSeconds_ += dt;

  if (currentSkill_ == Skill::Dash) {
    RefOwner().RefTransform().translate +=
        dashDirection_ * (params_.dashSpeedMetersPerSecond * dt);
    effectTimerSeconds_ += dt;
    if (effectTimerSeconds_ >= 0.08f) {
      effectTimerSeconds_ = 0.0f;
      const Math::Vector3 pos = RefOwner().GetWorldPosition();
      Particle::Combat::PlayOneShotParticleFile(
          kFxDashTrail, Math::Vector3{pos.x, 0.2f, pos.z});
    }
    ApplyVisual(1.0f, Math::Vector3{}, 1.0f, 0.65f, 0.1f, 0.3f);
  } else {
    if (!executeActionDone_) {
      executeActionDone_ = true;
      if (currentSkill_ == Skill::RingBlast) {
        ExecuteRingBlast();
      } else {
        ExecuteSummon();
      }
    }
    // 技後の硬直: 膨らんで戻る。
    const float t = std::clamp(stateElapsedSeconds_ / executeDurationSeconds_,
                               0.0f, 1.0f);
    ApplyVisual(1.0f + 0.15f * (1.0f - t), Math::Vector3{}, params_.colorR,
                params_.colorG, params_.colorB, 0.6f * (1.0f - t));
  }

  if (stateElapsedSeconds_ >= executeDurationSeconds_) {
    FinishSkillToWalking();
  }
}

void CmpAtomFBoss::UpdateStaggered(const float dt) {
  stateElapsedSeconds_ += dt;
  const float t = stateElapsedSeconds_ / params_.staggerSeconds;

  // 膝を突いた表現: 小さく沈み、ふらつく。水色の汗を頭から出す。
  const float sway = std::sin(stateElapsedSeconds_ * 9.0f) * 0.06f;
  ApplyVisual(0.88f, Math::Vector3{sway, -0.12f, 0.0f}, 0.35f, 0.55f, 0.95f,
              hitFlashSeconds_ > 0.0f ? 0.8f : 0.0f);

  effectTimerSeconds_ += dt;
  if (effectTimerSeconds_ >= 0.18f) {
    effectTimerSeconds_ = 0.0f;
    const Math::Vector3 pos = RefOwner().GetWorldPosition();
    static uint32_t sweatSequence = 0U;
    Particle::Combat::PlayEnemyPanicSweatEffect(
        Math::Vector3{pos.x, pos.y + params_.bodyRadiusMeters, pos.z},
        ++sweatSequence + 100000U);
  }

  if (t >= 1.0f) {
    FinishSkillToWalking();
  }
}

void CmpAtomFBoss::UpdateRoaring(const float dt) {
  stateElapsedSeconds_ += dt;
  const float t = std::clamp(stateElapsedSeconds_ / kRoarSeconds, 0.0f, 1.0f);

  const float phase = stateElapsedSeconds_ * 60.0f;
  const float amp = 0.1f;
  ApplyVisual(1.0f + 0.2f * std::sin(t * kPi),
              Math::Vector3{std::sin(phase) * amp, 0.0f,
                            std::cos(phase * 1.1f) * amp},
              Lerp(params_.colorR, 0.95f, t), Lerp(params_.colorG, 0.15f, t),
              Lerp(params_.colorB, 0.2f, t), 0.3f * (1.0f - t));

  effectTimerSeconds_ += dt;
  if (effectTimerSeconds_ >= 0.4f) {
    effectTimerSeconds_ = 0.0f;
    const Math::Vector3 pos = RefOwner().GetWorldPosition();
    // 咆哮の衝撃波（ダメージなしの演出）。
    Particle::Combat::PlayOneShotParticleFile(
        kFxShockwave, Math::Vector3{pos.x, 0.1f, pos.z});
    CombatCameraShake::Instance().RequestBossHitShake();
  }

  if (stateElapsedSeconds_ >= kRoarSeconds) {
    FinishSkillToWalking();
  }
}

void CmpAtomFBoss::UpdateDying(const float dt) {
  stateElapsedSeconds_ += dt;
  const float t = std::clamp(stateElapsedSeconds_ / params_.deathSeconds, 0.0f,
                             1.0f);

  // 震えが強まり、白赤に点滅しながら最後の 3 割で縮んで消える。
  const float phase = stateElapsedSeconds_ * 70.0f;
  const float amp = 0.05f + 0.2f * t;
  const float shrink = t < 0.7f ? 1.0f : 1.0f - (t - 0.7f) / 0.3f;
  const bool flashWhite =
      static_cast<int>(stateElapsedSeconds_ * 16.0f) % 2 == 0;
  ApplyVisual(shrink,
              Math::Vector3{std::sin(phase) * amp, 0.0f,
                            std::cos(phase * 1.2f) * amp},
              0.95f, 0.15f, 0.2f, flashWhite ? 0.85f : 0.0f);

  dyingExplosionTimerSeconds_ += dt;
  if (dyingExplosionTimerSeconds_ >= kDyingExplosionIntervalSeconds) {
    dyingExplosionTimerSeconds_ = 0.0f;
    const Math::Vector3 pos = RefOwner().GetWorldPosition();
    const float r = params_.bodyRadiusMeters;
    const Math::Vector3 offset{(NextRandom01() - 0.5f) * 2.0f * r,
                               (NextRandom01() - 0.2f) * r,
                               (NextRandom01() - 0.5f) * 2.0f * r};
    static uint32_t deathSequence = 0U;
    Particle::Combat::PlayBossDeathExplosionEffect(pos + offset,
                                                   ++deathSequence + 200000U);
    CombatCameraShake::Instance().RequestBossHitShake();
  }

  if (stateElapsedSeconds_ >= params_.deathSeconds) {
    const Math::Vector3 pos = RefOwner().GetWorldPosition();
    static uint32_t finalSequence = 0U;
    for (int i = 0; i < 3; ++i) {
      const float angle = kTwoPi * static_cast<float>(i) / 3.0f;
      Particle::Combat::PlayBossDeathExplosionEffect(
          Math::Vector3{pos.x + std::cos(angle) * 0.6f, pos.y,
                        pos.z + std::sin(angle) * 0.6f},
          ++finalSequence + 300000U);
    }
    Particle::Combat::PlayOneShotParticleFile(
        kFxShockwave, Math::Vector3{pos.x, 0.1f, pos.z});
    CombatCameraShake::Instance().RequestPlayerHitShake();
    RefOwner().Finalize();
    RefOwner().RequestDestroy();
  }
}

void CmpAtomFBoss::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  const float dt = frameDeltaTime_->GetDeltaTime();

  if (hitFlashSeconds_ > 0.0f) {
    hitFlashSeconds_ = std::max(0.0f, hitFlashSeconds_ - dt);
  }
  if (staggerCooldownRemainingSeconds_ > 0.0f) {
    staggerCooldownRemainingSeconds_ =
        std::max(0.0f, staggerCooldownRemainingSeconds_ - dt);
  }
  if (staggerAccumulatedDamage_ > 0.0f && params_.staggerWindowSeconds > 0.0f) {
    // 一定時間ダメージが入らなければ蓄積が抜ける（短時間の集中攻撃だけがダウンに繋がる）。
    staggerAccumulatedDamage_ = std::max(
        0.0f, staggerAccumulatedDamage_ -
                  params_.staggerDamageThreshold /
                      params_.staggerWindowSeconds * dt);
  }

  switch (state_) {
  case State::Appearing:
    UpdateAppearing(dt);
    break;
  case State::Walking:
    SetHitActive(true);
    UpdateWalking(dt);
    break;
  case State::Telegraph:
    UpdateTelegraph(dt);
    break;
  case State::Executing:
    UpdateExecuting(dt);
    break;
  case State::Staggered:
    UpdateStaggered(dt);
    break;
  case State::Roaring:
    UpdateRoaring(dt);
    break;
  case State::Dying:
    UpdateDying(dt);
    break;
  }
}

} // namespace Atrum
