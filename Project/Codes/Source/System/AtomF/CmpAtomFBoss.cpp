#include "System/AtomF/CmpAtomFBoss.h"

#include "Scene/SceneBuildUpAtomFEnemySpawn.h"
#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFEnemyProjectileSpawn.h"
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
#include <cstddef>
#include <string>

namespace Atrum {

namespace {

CmpAtomFBoss *gActiveBoss = nullptr;

constexpr float kPi = 3.14159265359f;
constexpr float kTwoPi = 6.28318530718f;
constexpr float kBossHitFlashSeconds = 0.12f;
constexpr float kSlamLeapSeconds = 0.45f;
constexpr float kSlamLeapPeakMeters = 3.2f;
constexpr float kAppearDropMeters = 9.0f;
constexpr float kEnrageSeconds = 1.3f;
constexpr float kDashSafetySeconds = 3.0f;
constexpr int kMaxMinionsForSummon = 4;

float SmoothStep(const float t) {
  const float c = std::clamp(t, 0.0f, 1.0f);
  return c * c * (3.0f - 2.0f * c);
}

float DistanceXZ(const Math::Vector3 &a, const Math::Vector3 &b) {
  const float dx = a.x - b.x;
  const float dz = a.z - b.z;
  return std::sqrt(dx * dx + dz * dz);
}

enum SkillId { kSkillRing = 0, kSkillSummon = 1, kSkillSlam = 2 };

} // namespace

CmpAtomFBoss *CmpAtomFBoss::Active() { return gActiveBoss; }

float CmpAtomFBoss::NextRandom01() {
  // xorshift32。決定論的な戦闘にして調整しやすくする。
  rngState_ ^= rngState_ << 13;
  rngState_ ^= rngState_ >> 17;
  rngState_ ^= rngState_ << 5;
  return static_cast<float>(rngState_ & 0xFFFFFFu) /
         static_cast<float>(0x1000000u);
}

void CmpAtomFBoss::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  params_ = AtomF::ResolveBossParameters();
  bodyRadius_ = params_.bodyRadiusMeters;
  health_ = params_.maxHealth;
  hitSphere_ = RefOwner().HasUpdCmp<CmpHitSphere>()
                   ? RefOwner().GetUpdCmp<CmpHitSphere>()
                   : nullptr;
  mesh_ = RefOwner().HasDrawCmp<CmpMeshSphere>()
              ? RefOwner().GetDrawCmp<CmpMeshSphere>()
              : nullptr;
  baseColor_ = Math::Vector4{params_.colorR, params_.colorG, params_.colorB,
                             1.0f};
}

void CmpAtomFBoss::Initialize() {
  gActiveBoss = this;
  state_ = State::Appear;
  stateTime_ = 0.0f;
  SetHitEnabled(false);
}

void CmpAtomFBoss::Finalize() {
  if (gActiveBoss == this) {
    gActiveBoss = nullptr;
  }
}

void CmpAtomFBoss::SetHitEnabled(const bool enabled) {
  if (hitSphere_ != nullptr) {
    hitSphere_->SetIsActive(enabled);
  }
}

float CmpAtomFBoss::TelegraphProgress() const {
  float duration = 0.0f;
  switch (state_) {
  case State::RingTelegraph:
    duration = params_.ringTelegraphSeconds;
    break;
  case State::SummonTelegraph:
    duration = params_.summonTelegraphSeconds;
    break;
  case State::SlamTelegraph:
    duration = params_.slamTelegraphSeconds;
    break;
  case State::DashTelegraph:
    duration = params_.dashTelegraphSeconds;
    break;
  case State::Enrage:
    duration = kEnrageSeconds;
    break;
  default:
    return 0.0f;
  }
  return std::clamp(stateTime_ / duration, 0.0f, 1.0f);
}

int CmpAtomFBoss::CurrentRingBulletCount() const {
  return phase2_ ? params_.phase2RingBulletCount : params_.ringBulletCount;
}

Math::Vector3 CmpAtomFBoss::PlayerPosition(bool &hasPlayer) const {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    hasPlayer = false;
    return Math::Vector3{};
  }
  hasPlayer = true;
  return player->GetWorldPosition();
}

int CmpAtomFBoss::CountMinions() {
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return 0;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  int count = 0;
  for (CmpHitSphere *hit : judge->RegisteredHitSpheres()) {
    if (hit == nullptr) {
      continue;
    }
    Entity *other = &hit->RefOwner();
    if (other == &RefOwner() || !other->IsActive()) {
      continue;
    }
    if (tags.EntityHasRole(*other, "enemy")) {
      ++count;
    }
  }
  return count;
}

bool CmpAtomFBoss::TakeDamage(const DamageSource source) {
  if (state_ == State::Appear || state_ == State::Dying || health_ <= 0) {
    return false;
  }
  const int base = source == DamageSource::PlayerBullet
                       ? params_.playerBulletDamage
                       : params_.explosionDamage;
  float scaled = static_cast<float>(base);
  if (state_ == State::Stunned) {
    scaled *= params_.stunDamageMultiplier;
  }
  const int amount = std::max(1, static_cast<int>(scaled + 0.5f));
  health_ = std::max(0, health_ - amount);
  hitFlashTimer_ = kBossHitFlashSeconds;
  Particle::Combat::PlayBossHitByReflectedBulletEffect(
      RefOwner().GetWorldPosition());

  if (!phase2_ &&
      static_cast<float>(health_) <=
          static_cast<float>(params_.maxHealth) * params_.phase2HealthRatio) {
    phase2_ = true;
  }
  if (health_ <= 0) {
    BeginDeath();
  }
  return true;
}

void CmpAtomFBoss::EnterState(const State next) {
  state_ = next;
  stateTime_ = 0.0f;
  auraTimer_ = 0.0f;
  RefOwner().RefTransform().translate.y = bodyRadius_;
}

void CmpAtomFBoss::BeginDeath() {
  EnterState(State::Dying);
  SetHitEnabled(false);
  deathBurstTimer_ = 0.0f;
  deathFinalBurstPlayed_ = false;

  // 残った子分は静かに誘爆させる（自機にはダメージを与えない）。
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  for (CmpHitSphere *hit : judge->RegisteredHitSpheres()) {
    if (hit == nullptr) {
      continue;
    }
    Entity *other = &hit->RefOwner();
    if (other == &RefOwner() || !other->IsActive() ||
        !tags.EntityHasRole(*other, "enemy")) {
      continue;
    }
    if (other->HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
      if (CmpAtomFEnemyChainExplosion *chain =
              other->GetUpdCmp<CmpAtomFEnemyChainExplosion>()) {
        chain->BeginSilentDefeat();
      }
    }
  }
}

void CmpAtomFBoss::PlayAuraIfDue(const float dt, const float interval) {
  auraTimer_ -= dt;
  if (auraTimer_ > 0.0f) {
    return;
  }
  auraTimer_ = interval;
  Math::Vector3 pos = RefOwner().GetWorldPosition();
  pos.y = bodyRadius_ * 0.8f;
  Particle::Combat::PlayBossChargeAuraEffect(pos);
}

void CmpAtomFBoss::ChaseTowardPlayer(const float dt, const float stopDistance) {
  bool hasPlayer = false;
  const Math::Vector3 playerPos = PlayerPosition(hasPlayer);
  if (!hasPlayer) {
    return;
  }
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  Math::Vector3 toPlayer = playerPos - translate;
  toPlayer.y = 0.0f;
  const float dist = std::sqrt(toPlayer.LengthSquare());
  if (dist <= stopDistance || dist < 1e-4f) {
    return;
  }
  toPlayer *= (1.0f / dist);
  translate += toPlayer * (params_.moveSpeedMetersPerSecond * dt);
}

void CmpAtomFBoss::ChooseNextSkill() {
  ++skillCount_;
  if (skillCount_ % params_.dashEveryNSkills == 0) {
    EnterState(State::DashTelegraph);
    bool hasPlayer = false;
    const Math::Vector3 playerPos = PlayerPosition(hasPlayer);
    Math::Vector3 dir = hasPlayer
                            ? playerPos - RefOwner().RefTransform().translate
                            : Math::Vector3{0.0f, 0.0f, 1.0f};
    dir.y = 0.0f;
    if (dir.LengthSquare() > 1e-6f) {
      dir.Normalize();
    } else {
      dir = Math::Vector3{0.0f, 0.0f, 1.0f};
    }
    dashDirection_ = dir;
    return;
  }

  int pick = static_cast<int>(NextRandom01() * 3.0f);
  pick = std::clamp(pick, 0, 2);
  if (pick == lastSkill_) {
    pick = (pick + 1) % 3;
  }
  if (pick == kSkillSummon && CountMinions() >= kMaxMinionsForSummon) {
    pick = (NextRandom01() < 0.5f) ? kSkillRing : kSkillSlam;
  }
  lastSkill_ = pick;

  switch (pick) {
  case kSkillRing:
    EnterState(State::RingTelegraph);
    burstsFired_ = 0;
    break;
  case kSkillSummon: {
    EnterState(State::SummonTelegraph);
    summonPoints_.clear();
    const int count =
        phase2_ ? params_.phase2SummonCount : params_.summonCount;
    const float arenaRadius = AtomF::ResolveArenaInnerRadiusMeters();
    const float baseAngle = NextRandom01() * kTwoPi;
    const float ringRadius =
        std::min(bodyRadius_ + 2.4f, arenaRadius - 1.0f);
    for (int i = 0; i < count; ++i) {
      const float angle =
          baseAngle + kTwoPi * static_cast<float>(i) / static_cast<float>(count);
      const Math::Vector3 center = RefOwner().RefTransform().translate;
      Math::Vector3 p{center.x + std::cos(angle) * ringRadius, 0.0f,
                      center.z + std::sin(angle) * ringRadius};
      AtomF::ClampWorldTranslateToArenaDisc(p, 0.6f);
      summonPoints_.push_back(p);
    }
    break;
  }
  default: {
    EnterState(State::SlamTelegraph);
    bool hasPlayer = false;
    Math::Vector3 target = PlayerPosition(hasPlayer);
    if (!hasPlayer) {
      target = RefOwner().RefTransform().translate;
    }
    target.y = bodyRadius_;
    AtomF::ClampWorldTranslateToArenaDisc(target, bodyRadius_);
    slamTarget_ = target;
    slamStart_ = RefOwner().RefTransform().translate;
    break;
  }
  }
}

void CmpAtomFBoss::UpdateAppear(const float dt) {
  (void)dt;
  const float t = std::clamp(stateTime_ / params_.appearSeconds, 0.0f, 1.0f);
  if (!appearFxPlayed_) {
    appearFxPlayed_ = true;
    Math::Vector3 pos = RefOwner().GetWorldPosition();
    pos.y = 0.05f;
    Particle::Combat::PlayBossAppearEffect(pos);
  }
  if (!landFxPlayed_ && t >= 0.8f) {
    landFxPlayed_ = true;
    Math::Vector3 pos = RefOwner().GetWorldPosition();
    pos.y = 0.05f;
    Particle::Combat::PlayBossSlamShockwaveEffect(pos);
  }
  if (t >= 1.0f) {
    EnterState(State::Idle);
    skillTimer_ = params_.skillIntervalSeconds;
    SetHitEnabled(true);
  }
}

void CmpAtomFBoss::UpdateIdle(const float dt) {
  ChaseTowardPlayer(dt, bodyRadius_ + 1.6f);
  if (phase2_ && !phase2Announced_) {
    phase2Announced_ = true;
    EnterState(State::Enrage);
    return;
  }
  skillTimer_ -= dt;
  if (skillTimer_ <= 0.0f) {
    ChooseNextSkill();
  }
}

void CmpAtomFBoss::UpdateEnrage(const float dt) {
  PlayAuraIfDue(dt, 0.12f);
  if (stateTime_ >= kEnrageSeconds) {
    Math::Vector3 pos = RefOwner().GetWorldPosition();
    pos.y = 0.05f;
    Particle::Combat::PlayBossSlamShockwaveEffect(pos);
    EnterState(State::Idle);
    skillTimer_ = params_.phase2SkillIntervalSeconds;
  }
}

void CmpAtomFBoss::FireRingBurst() {
  const int count = CurrentRingBulletCount();
  // 偶数回目は半ピッチずらして隙間を変える。初回は 1 発が自機を向く。
  float offset = (burstsFired_ % 2 == 0) ? 0.0f : kPi / static_cast<float>(count);
  if (burstsFired_ == 0) {
    bool hasPlayer = false;
    const Math::Vector3 playerPos = PlayerPosition(hasPlayer);
    if (hasPlayer) {
      const Math::Vector3 d = playerPos - RefOwner().GetWorldPosition();
      offset = std::atan2(d.z, d.x);
    }
  }
  Math::Vector3 origin = RefOwner().GetWorldPosition();
  origin.y = AtomF::ResolveEnemyBodyRadiusMeters();
  Particle::Combat::PlayEnemyMuzzleFlashEffect(origin);
  for (int i = 0; i < count; ++i) {
    const float angle =
        offset + kTwoPi * static_cast<float>(i) / static_cast<float>(count);
    const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
    AtomF::SpawnEnemyProjectile(origin + dir * (bodyRadius_ + 0.1f), dir,
                                1.0f, 0.35f, 0.9f);
  }
  ++burstsFired_;
}

void CmpAtomFBoss::UpdateRing(const float dt) {
  if (state_ == State::RingTelegraph) {
    PlayAuraIfDue(dt, 0.2f);
    if (stateTime_ >= params_.ringTelegraphSeconds) {
      EnterState(State::RingFire);
      burstsFired_ = 0;
      FireRingBurst();
      burstTimer_ = params_.ringBurstIntervalSeconds;
    }
    return;
  }
  burstTimer_ -= dt;
  if (burstTimer_ <= 0.0f) {
    if (burstsFired_ >= params_.ringBurstCount) {
      EnterState(State::Idle);
      skillTimer_ =
          phase2_ ? params_.phase2SkillIntervalSeconds : params_.skillIntervalSeconds;
      return;
    }
    FireRingBurst();
    burstTimer_ = params_.ringBurstIntervalSeconds;
  }
}

void CmpAtomFBoss::ExecuteSummon() {
  const AtomF::ParametersRuntime &runtime = AtomF::ParametersRuntime::Instance();
  const std::size_t variantCount =
      runtime.IsLoaded() ? runtime.Enemy().variants.size() : 0U;
  for (const Math::Vector3 &p : summonPoints_) {
    std::string variantName;
    if (variantCount > 0U) {
      const std::size_t index = std::min(
          variantCount - 1U,
          static_cast<std::size_t>(NextRandom01() *
                                   static_cast<float>(variantCount)));
      variantName = runtime.Enemy().variants[index].name;
    }
    EnqueueAtomFEnemy(variantName, p.x, p.z);
  }
  summonPoints_.clear();
}

void CmpAtomFBoss::UpdateSummon(const float dt) {
  PlayAuraIfDue(dt, 0.15f);
  if (stateTime_ >= params_.summonTelegraphSeconds) {
    ExecuteSummon();
    EnterState(State::Idle);
    skillTimer_ =
        phase2_ ? params_.phase2SkillIntervalSeconds : params_.skillIntervalSeconds;
  }
}

void CmpAtomFBoss::LandSlam() {
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate = slamTarget_;
  translate.y = bodyRadius_;
  Math::Vector3 fxPos = translate;
  fxPos.y = 0.05f;
  Particle::Combat::PlayBossSlamShockwaveEffect(fxPos);

  bool hasPlayer = false;
  const Math::Vector3 playerPos = PlayerPosition(hasPlayer);
  if (!hasPlayer) {
    return;
  }
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (DistanceXZ(playerPos, translate) <=
      params_.slamRadiusMeters + AtomF::ResolvePlayerBodyRadiusMeters()) {
    if (player != nullptr && player->HasUpdCmp<CmpHealthHit>()) {
      if (CmpHealthHit *health = player->GetUpdCmp<CmpHealthHit>()) {
        health->TryTakeDamageUnconditional();
      }
    }
  }
}

void CmpAtomFBoss::UpdateSlam(const float dt) {
  (void)dt;
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  if (state_ == State::SlamTelegraph) {
    if (stateTime_ >= params_.slamTelegraphSeconds) {
      EnterState(State::SlamLeap);
    }
    return;
  }
  if (state_ == State::SlamLeap) {
    const float t = std::clamp(stateTime_ / kSlamLeapSeconds, 0.0f, 1.0f);
    const float e = SmoothStep(t);
    translate.x = slamStart_.x + (slamTarget_.x - slamStart_.x) * e;
    translate.z = slamStart_.z + (slamTarget_.z - slamStart_.z) * e;
    if (t >= 1.0f) {
      LandSlam();
      EnterState(State::SlamRecover);
    }
    return;
  }
  if (stateTime_ >= params_.slamRecoverSeconds) {
    EnterState(State::Idle);
    skillTimer_ =
        phase2_ ? params_.phase2SkillIntervalSeconds : params_.skillIntervalSeconds;
  }
}

void CmpAtomFBoss::UpdateDash(const float dt) {
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  if (state_ == State::DashTelegraph) {
    PlayAuraIfDue(dt, 0.1f);
    // 前半は自機を追って狙いを定め、後半は固定して「避けられる」猶予を作る。
    if (TelegraphProgress() < 0.6f) {
      bool hasPlayer = false;
      const Math::Vector3 playerPos = PlayerPosition(hasPlayer);
      if (hasPlayer) {
        Math::Vector3 dir = playerPos - translate;
        dir.y = 0.0f;
        if (dir.LengthSquare() > 1e-6f) {
          dir.Normalize();
          dashDirection_ = dir;
        }
      }
    }
    if (stateTime_ >= params_.dashTelegraphSeconds) {
      EnterState(State::Dashing);
      dashDustTimer_ = 0.0f;
    }
    return;
  }

  translate += dashDirection_ * (params_.dashSpeedMetersPerSecond * dt);
  dashDustTimer_ -= dt;
  if (dashDustTimer_ <= 0.0f) {
    dashDustTimer_ = 0.08f;
    Math::Vector3 dust = translate;
    dust.y = 0.05f;
    Particle::Combat::PlayBossDashDustEffect(dust);
  }
  const bool hitWall =
      AtomF::IsWorldXZTouchingArenaWall(translate.x, translate.z, bodyRadius_);
  if (hitWall || stateTime_ >= kDashSafetySeconds) {
    AtomF::ClampWorldTranslateToArenaDisc(translate, bodyRadius_);
    Math::Vector3 impact = translate + dashDirection_ * bodyRadius_;
    impact.y = bodyRadius_;
    Particle::Combat::PlayBossWallImpactEffect(impact);
    EnterState(State::Stunned);
  }
}

void CmpAtomFBoss::UpdateStunned(const float dt) {
  (void)dt;
  if (stateTime_ >= params_.dashStunSeconds) {
    EnterState(State::Idle);
    skillTimer_ =
        phase2_ ? params_.phase2SkillIntervalSeconds : params_.skillIntervalSeconds;
  }
}

void CmpAtomFBoss::UpdateDying(const float dt) {
  deathBurstTimer_ -= dt;
  if (deathBurstTimer_ <= 0.0f) {
    deathBurstTimer_ = 0.22f;
    const float angle = NextRandom01() * kTwoPi;
    const float r = NextRandom01() * bodyRadius_;
    Math::Vector3 pos = RefOwner().GetWorldPosition();
    pos.x += std::cos(angle) * r;
    pos.z += std::sin(angle) * r;
    pos.y = bodyRadius_ * (0.4f + NextRandom01());
    static uint32_t deathSequence = 100000U;
    Particle::Combat::PlayBossDeathExplosionEffect(pos, ++deathSequence);
  }
  if (stateTime_ >= params_.deathSeconds) {
    if (!deathFinalBurstPlayed_) {
      deathFinalBurstPlayed_ = true;
      static uint32_t finalSequence = 200000U;
      Math::Vector3 pos = RefOwner().GetWorldPosition();
      Particle::Combat::PlayBossDeathExplosionEffect(pos, ++finalSequence);
      pos.y = 0.05f;
      Particle::Combat::PlayBossSlamShockwaveEffect(pos);
    }
    destroyed_ = true;
    RefOwner().Finalize();
    RefOwner().RequestDestroy();
  }
}

void CmpAtomFBoss::ApplyVisual() {
  if (mesh_ == nullptr) {
    return;
  }
  const float time = stateTime_;
  Math::Vector3 offset{};
  float radiusScale = 1.0f;
  Math::Vector4 color = baseColor_;

  auto mix = [&color](const Math::Vector4 &to, const float k) {
    color.x += (to.x - color.x) * k;
    color.y += (to.y - color.y) * k;
    color.z += (to.z - color.z) * k;
  };
  const Math::Vector4 warning{1.0f, 0.3f, 0.2f, 1.0f};
  const float pulse = 0.5f + 0.5f * std::sin(time * 22.0f);
  const float progress = TelegraphProgress();

  switch (state_) {
  case State::Appear: {
    const float t = std::clamp(time / params_.appearSeconds, 0.0f, 1.0f);
    offset.y = (1.0f - SmoothStep((t - 0.1f) / 0.7f)) * kAppearDropMeters;
    radiusScale = 0.4f + 0.6f * SmoothStep(t / 0.8f);
    mix(Math::Vector4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f - t);
    break;
  }
  case State::Enrage:
    offset.x = std::sin(time * 60.0f) * 0.08f;
    offset.z = std::cos(time * 52.0f) * 0.08f;
    mix(warning, 0.4f + 0.4f * pulse);
    radiusScale = 1.0f + 0.12f * pulse;
    break;
  case State::RingTelegraph:
  case State::SummonTelegraph:
  case State::DashTelegraph:
    offset.x = std::sin(time * 55.0f) * (0.03f + 0.1f * progress);
    offset.z = std::cos(time * 47.0f) * (0.03f + 0.1f * progress);
    mix(warning, progress * (0.5f + 0.5f * pulse));
    radiusScale = 1.0f + 0.1f * progress * pulse;
    break;
  case State::SlamTelegraph:
    offset.y = -0.25f * progress; // しゃがみ込み
    offset.x = std::sin(time * 50.0f) * 0.05f;
    mix(warning, progress * (0.4f + 0.6f * pulse));
    radiusScale = 1.0f - 0.12f * progress;
    break;
  case State::SlamLeap: {
    const float t = std::clamp(time / kSlamLeapSeconds, 0.0f, 1.0f);
    offset.y = std::sin(t * kPi) * kSlamLeapPeakMeters;
    break;
  }
  case State::SlamRecover:
    radiusScale = 1.0f - 0.1f * (1.0f - std::clamp(time / params_.slamRecoverSeconds, 0.0f, 1.0f));
    break;
  case State::Dashing:
    mix(warning, 0.6f);
    break;
  case State::Stunned:
    offset.x = std::sin(time * 30.0f) * 0.12f;
    offset.z = std::cos(time * 27.0f) * 0.12f;
    mix(Math::Vector4{0.6f, 0.9f, 1.0f, 1.0f}, 0.55f + 0.25f * pulse);
    break;
  case State::Dying: {
    const float t = std::clamp(time / params_.deathSeconds, 0.0f, 1.0f);
    offset.x = std::sin(time * 70.0f) * 0.2f * (1.0f - t * 0.5f);
    offset.z = std::cos(time * 63.0f) * 0.2f * (1.0f - t * 0.5f);
    mix(Math::Vector4{1.0f, 0.95f, 0.8f, 1.0f}, t);
    // 終盤は急速に縮んで消える。
    radiusScale = t < 0.85f ? 1.0f : std::max(0.0f, 1.0f - (t - 0.85f) / 0.15f);
    color.w = t < 0.85f ? 1.0f : radiusScale;
    break;
  }
  default:
    break;
  }

  if (hitFlashTimer_ > 0.0f) {
    mix(Math::Vector4{1.0f, 1.0f, 1.0f, 1.0f}, hitFlashTimer_ / kBossHitFlashSeconds);
  }

  mesh_->SetDrawOffset(offset);
  mesh_->SetRadius(bodyRadius_ * radiusScale);
  mesh_->SetColor(color);
}

void CmpAtomFBoss::Update() {
  if (frameDeltaTime_ == nullptr || destroyed_) {
    return;
  }
  const float dt = frameDeltaTime_->GetDeltaTime();
  stateTime_ += dt;
  hitFlashTimer_ = std::max(0.0f, hitFlashTimer_ - dt);

  switch (state_) {
  case State::Appear:
    UpdateAppear(dt);
    break;
  case State::Idle:
    UpdateIdle(dt);
    break;
  case State::Enrage:
    UpdateEnrage(dt);
    break;
  case State::RingTelegraph:
  case State::RingFire:
    UpdateRing(dt);
    break;
  case State::SummonTelegraph:
    UpdateSummon(dt);
    break;
  case State::SlamTelegraph:
  case State::SlamLeap:
  case State::SlamRecover:
    UpdateSlam(dt);
    break;
  case State::DashTelegraph:
  case State::Dashing:
    UpdateDash(dt);
    break;
  case State::Stunned:
    UpdateStunned(dt);
    break;
  case State::Dying:
    UpdateDying(dt);
    break;
  }

  if (destroyed_) {
    return;
  }
  ApplyVisual();
}

} // namespace Atrum
