#include "System/AtomF/CmpAtomFBoss.h"

#include "Camera/CombatCameraShake.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFBossShockwave.h"
#include "System/AtomF/AtomFEnemyFactory.h"
#include "System/AtomF/AtomFEnemyProjectileSpawn.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/AtomFSe.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpDrawAtomFTelegraph.h"
#include "System/DeltaTime.h"
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

constexpr float kPi = 3.14159265359f;
constexpr float kTwoPi = 6.28318530718f;
/// 突進・連射の狙いを固定する、発動直前の秒数。
constexpr float kAimLockSeconds = 0.25f;
constexpr float kBossHitFlashSeconds = 0.12f;

CmpAtomFBoss *g_currentBoss = nullptr;

/// 行動の順番（固定）。種類を散らしつつ、突進を挟んで位置取りを崩す。
constexpr CmpAtomFBoss::Skill kSkillSequence[] = {
    CmpAtomFBoss::Skill::Volley, CmpAtomFBoss::Skill::Ring,
    CmpAtomFBoss::Skill::Dash,   CmpAtomFBoss::Skill::Summon,
    CmpAtomFBoss::Skill::Ring,   CmpAtomFBoss::Skill::Dash,
    CmpAtomFBoss::Skill::SummonFlank, CmpAtomFBoss::Skill::GapRing,
    CmpAtomFBoss::Skill::Sweep,  CmpAtomFBoss::Skill::Slam,
    CmpAtomFBoss::Skill::Fan,    CmpAtomFBoss::Skill::Volley};

/// 狂暴化（Hp 40% 以下）後の行動順。スパイラルを織り込み、密度を上げる。
constexpr CmpAtomFBoss::Skill kEnragedSkillSequence[] = {
    CmpAtomFBoss::Skill::Spiral, CmpAtomFBoss::Skill::Dash,
    CmpAtomFBoss::Skill::Volley, CmpAtomFBoss::Skill::Ring,
    CmpAtomFBoss::Skill::SummonSurround, CmpAtomFBoss::Skill::Spiral,
    CmpAtomFBoss::Skill::Dash,   CmpAtomFBoss::Skill::SummonFlank,
    CmpAtomFBoss::Skill::Sweep,  CmpAtomFBoss::Skill::GapRing,
    CmpAtomFBoss::Skill::Fan,    CmpAtomFBoss::Skill::Slam,
    CmpAtomFBoss::Skill::Ring};

/// スパイラル: 回転しながら撃つ弾幕。
constexpr int kSpiralShotCount = 36;
constexpr float kSpiralIntervalSeconds = 0.07f;
constexpr float kSpiralTurnPerShot = 0.38f;
constexpr float kSpiralTelegraphSeconds = 1.0f;
constexpr float kSpiralBulletSpeed = 2.4f;

/// 隙間リング: 全方位弾のうち、自機の向きから 90° ずれた一方向だけ弾を空ける。
constexpr int kGapRingBulletCount = 28;
constexpr float kGapRingHalfWidthRadians = 0.42f;
constexpr float kGapRingTelegraphSeconds = 1.1f;
constexpr float kGapRingBulletSpeed = 2.3f;

/// 薙ぎ払い: 自機方向を中心に扇状へ弾列を振る。
constexpr int kSweepShotCount = 22;
constexpr float kSweepIntervalSeconds = 0.06f;
constexpr float kSweepHalfArcRadians = 1.05f;
constexpr float kSweepTelegraphSeconds = 1.0f;
constexpr float kSweepBulletSpeed = 2.6f;

/// 扇弾3連: 自機狙いの扇状の弾を、毎波狙い直して連続で撃つ。
constexpr int kFanWaveCount = 3;
constexpr int kFanBulletsPerWave = 7;
constexpr float kFanHalfArcRadians = 0.55f;
constexpr float kFanWaveIntervalSeconds = 0.55f;
constexpr float kFanTelegraphSeconds = 0.9f;
constexpr float kFanBulletSpeed = 3.0f;

/// 跳躍叩きつけ: 自機の足元へ跳び、着地点で小さな衝撃波を起こす（近接）。
constexpr float kSlamTelegraphSeconds = 1.0f;
constexpr float kSlamLeapSeconds = 0.65f;
constexpr float kSlamArcHeight = 2.6f;
constexpr float kSlamShockRadius = 4.2f;
constexpr float kSlamShockSpeed = 6.0f;
constexpr float kSlamShockBand = 0.3f;
constexpr float kSlamRecoverSeconds = 1.1f;
constexpr float kSlamTargetRadius = 1.6f;

/// 自機を囲む召喚の数・半径、両翼召喚の数・間隔。
constexpr int kSurroundCount = 5;
constexpr float kSurroundRadius = 4.2f;
constexpr int kFlankPerSide = 2;
constexpr float kFlankSideOffset = 3.2f;
constexpr float kFlankSpacing = 1.6f;

float Rand01() {
  static uint32_t state = 0x1234ABCDu;
  state = state * 1664525u + 1013904223u;
  return static_cast<float>((state >> 8) & 0xFFFFu) / 65535.0f;
}

float EaseOutBack(const float t) {
  constexpr float c1 = 1.70158f;
  constexpr float c3 = c1 + 1.0f;
  const float u = t - 1.0f;
  return 1.0f + c3 * u * u * u + c1 * u * u;
}

Math::Vector4 Lerp(const Math::Vector4 &a, const Math::Vector4 &b,
                   const float t) {
  return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
}

CmpMeshSphere *MeshOf(Entity &owner) {
  return owner.HasDrawCmp<CmpMeshSphere>() ? owner.GetDrawCmp<CmpMeshSphere>()
                                           : nullptr;
}

CmpDrawAtomFTelegraph *TelegraphOf(Entity &owner) {
  return owner.HasDrawCmp<CmpDrawAtomFTelegraph>()
             ? owner.GetDrawCmp<CmpDrawAtomFTelegraph>()
             : nullptr;
}

Math::Vector3 GroundPoint(const Math::Vector3 &p) { return {p.x, 0.0f, p.z}; }

} // namespace

CmpAtomFBoss *CmpAtomFBoss::Current() { return g_currentBoss; }

void CmpAtomFBoss::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  params_ = AtomF::ParametersRuntime::Instance().Boss();
  radius_ = params_.hitRadiusMeters;
}

CmpAtomFBoss::~CmpAtomFBoss() {
  if (g_currentBoss == this) {
    g_currentBoss = nullptr;
  }
}

void CmpAtomFBoss::Initialize() {
  health_ = params_.maxHealth;
  displayedHealthRatio_ = 1.0f;
  state_ = State::Entrance;
  stateElapsed_ = 0.0f;
  entranceEffectPlayed_ = false;
  barGlowPlayed_ = false;
  if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
    if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
      hit->SetIsActive(false);
    }
  }
}

void CmpAtomFBoss::Finalize() {
  if (g_currentBoss == this) {
    g_currentBoss = nullptr;
  }
}

float CmpAtomFBoss::HpBarReveal01() const {
  if (state_ != State::Entrance) {
    return 1.0f;
  }
  const float span = std::max(0.1f, params_.entranceSeconds * 0.8f);
  return std::clamp(stateElapsed_ / span, 0.0f, 1.0f);
}

Math::Vector3 CmpAtomFBoss::DirectionToPlayerXZ() const {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return lockedDirection_;
  }
  const Entity *self = GetOwner();
  Math::Vector3 toPlayer = player->GetWorldPosition() - self->GetWorldPosition();
  toPlayer.y = 0.0f;
  const float lengthSq = toPlayer.LengthSquare();
  if (lengthSq < 1e-6f) {
    return lockedDirection_;
  }
  return toPlayer * (1.0f / std::sqrt(lengthSq));
}

void CmpAtomFBoss::MoveToward(const Math::Vector3 &targetXZ, const float speed,
                              const float dt) {
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  Math::Vector3 to = targetXZ - translate;
  to.y = 0.0f;
  const float lengthSq = to.LengthSquare();
  if (lengthSq < 1e-6f) {
    return;
  }
  translate += to * (speed * dt / std::sqrt(lengthSq));
  translate.y = radius_;
}

bool CmpAtomFBoss::ApplyDamage(const int baseAmount,
                               const Math::Vector3 &hitPosition) {
  if (!IsDamageable() || health_ <= 0) {
    return false;
  }
  const float multiplier =
      state_ == State::Recover ? params_.recoverDamageMultiplier : 1.0f;
  const int amount = std::max(
      1, static_cast<int>(std::lround(static_cast<float>(baseAmount) * multiplier)));
  health_ = std::max(0, health_ - amount);
  hitFlashSeconds_ = kBossHitFlashSeconds;

  Particle::Combat::PlayBossHitByReflectedBulletEffect(hitPosition);
  AtomF::PlayBossHitSe();
  CombatCameraShake::Instance().RequestBossHitShake();

  if (!enraged_ &&
      static_cast<float>(health_) <=
          static_cast<float>(params_.maxHealth) * params_.enrageHealthRatio) {
    enraged_ = true;
    AtomF::PlayBossBulletWarnSe();
    CombatCameraShake::Instance().RequestCustomShake(0.22f, 0.7f);
    Particle::Combat::PlayNamedOneShotEffect("atomf_boss_enrage.json",
                                             RefOwner().GetWorldPosition());
  }

  if (health_ <= 0) {
    BeginDying();
  }
  return true;
}

void CmpAtomFBoss::ClearTelegraph() {
  if (CmpDrawAtomFTelegraph *telegraph = TelegraphOf(RefOwner())) {
    telegraph->Clear();
  }
}

void CmpAtomFBoss::SetDrawJitter(const float amplitude, const float time) {
  if (CmpMeshSphere *mesh = MeshOf(RefOwner())) {
    if (amplitude <= 0.0f) {
      mesh->SetDrawOffset(Math::Vector3{});
      return;
    }
    mesh->SetDrawOffset(Math::Vector3{std::sin(time * 83.0f) * amplitude, 0.0f,
                                      std::cos(time * 71.0f) * amplitude});
  }
}

void CmpAtomFBoss::FinishSkill() {
  state_ = State::Idle;
  stateElapsed_ = 0.0f;
  const float multiplier = enraged_ ? params_.enrageCooldownMultiplier : 1.0f;
  idleTimer_ = params_.skillCooldownSeconds * multiplier;
  ClearTelegraph();
  SetDrawJitter(0.0f, 0.0f);
}

void CmpAtomFBoss::BeginTelegraph(const Skill skill) {
  state_ = State::Telegraph;
  skill_ = skill;
  stateElapsed_ = 0.0f;
  lockedDirection_ = DirectionToPlayerXZ();

  if (skill == Skill::Dash || skill == Skill::Slam) {
    AtomF::PlayBossMeleeWarnSe();
  } else {
    AtomF::PlayBossBulletWarnSe();
  }

  if (skill == Skill::Slam) {
    slamTarget_ = GroundPoint(RefOwner().GetWorldPosition());
  }

  if (skill == Skill::Spiral) {
    spiralAngle_ = Rand01() * kTwoPi;
  }

  if (skill == Skill::GapRing) {
    // 自機の向きから左右どちらかへ 90° ずらした方向に隙間を作る（自機は隙間へ動く必要がある）。
    const float toPlayer = std::atan2(lockedDirection_.z, lockedDirection_.x);
    gapAngle_ = toPlayer + (Rand01() < 0.5f ? -0.5f : 0.5f) * kPi;
  }

  if (skill == Skill::Sweep) {
    // 自機方向を中心に、左右どちらから振り始めるかを決める。
    sweepDirection_ = Rand01() < 0.5f ? -1.0f : 1.0f;
    sweepStartAngle_ = std::atan2(lockedDirection_.z, lockedDirection_.x) -
                       sweepDirection_ * kSweepHalfArcRadians;
  }

  if (skill == Skill::SummonSurround) {
    // 自機を囲む輪の上に等間隔で召喚する（アリーナ内へ収める）。
    summonPoints_.clear();
    Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
    const Math::Vector3 playerPos =
        player != nullptr ? player->GetWorldPosition() : Math::Vector3{};
    const float arena = AtomF::ResolveArenaInnerRadiusMeters() * 0.9f;
    const float offset = Rand01() * kTwoPi;
    for (int i = 0; i < kSurroundCount; ++i) {
      const float angle =
          offset + kTwoPi * static_cast<float>(i) / kSurroundCount;
      Math::Vector3 point{playerPos.x + std::cos(angle) * kSurroundRadius, 0.0f,
                          playerPos.z + std::sin(angle) * kSurroundRadius};
      const float len = std::sqrt(point.x * point.x + point.z * point.z);
      if (len > arena && len > 1e-4f) {
        point = {point.x * arena / len, 0.0f, point.z * arena / len};
      }
      summonPoints_.push_back(point);
    }
  } else if (skill == Skill::SummonFlank) {
    // ボスの両脇（ボス→自機の向きに直交）へ縦隊で召喚する。
    summonPoints_.clear();
    const Math::Vector3 origin = GroundPoint(RefOwner().GetWorldPosition());
    const Math::Vector3 forward = lockedDirection_;
    const Math::Vector3 side{-forward.z, 0.0f, forward.x};
    const float arena = AtomF::ResolveArenaInnerRadiusMeters() * 0.9f;
    for (int sign = -1; sign <= 1; sign += 2) {
      for (int i = 0; i < kFlankPerSide; ++i) {
        Math::Vector3 point =
            origin + side * (static_cast<float>(sign) * kFlankSideOffset) +
            forward * (kFlankSpacing * static_cast<float>(i));
        point.y = 0.0f;
        const float len = std::sqrt(point.x * point.x + point.z * point.z);
        if (len > arena && len > 1e-4f) {
          point = {point.x * arena / len, 0.0f, point.z * arena / len};
        }
        summonPoints_.push_back(point);
      }
    }
  }

  if (skill == Skill::Summon) {
    summonPoints_.clear();
    Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
    const Math::Vector3 playerPos =
        player != nullptr ? player->GetWorldPosition() : Math::Vector3{};
    const float arena = AtomF::ResolveArenaInnerRadiusMeters();
    for (int i = 0; i < params_.summonCount; ++i) {
      Math::Vector3 point{};
      for (int attempt = 0; attempt < 8; ++attempt) {
        const float angle = Rand01() * kTwoPi;
        const float dist = arena * (0.35f + 0.5f * Rand01());
        point = {std::cos(angle) * dist, 0.0f, std::sin(angle) * dist};
        const float dx = point.x - playerPos.x;
        const float dz = point.z - playerPos.z;
        if (dx * dx + dz * dz >= 9.0f) {
          break;
        }
      }
      summonPoints_.push_back(point);
    }
  }
}

void CmpAtomFBoss::BeginDash() {
  state_ = State::Dash;
  stateElapsed_ = 0.0f;
  ClearTelegraph();
  SetDrawJitter(0.0f, 0.0f);
}

void CmpAtomFBoss::EndDash() {
  const Math::Vector3 ground = GroundPoint(RefOwner().GetWorldPosition());
  AtomF::SpawnBossShockwave(ground, params_.shockwaveMaxRadiusMeters,
                            params_.shockwaveSpeedMetersPerSecond,
                            params_.shockwaveBandHalfWidthMeters);
  Particle::Combat::PlayNamedOneShotEffect("atomf_boss_slam.json", ground);
  AtomF::PlayBossDeathBoomSe();
  CombatCameraShake::Instance().RequestCustomShake(0.32f, 0.55f);
  state_ = State::Recover;
  stateElapsed_ = 0.0f;
}

void CmpAtomFBoss::BeginLeap() {
  state_ = State::Leap;
  stateElapsed_ = 0.0f;
  leapStart_ = RefOwner().GetWorldPosition();
  ClearTelegraph();
  SetDrawJitter(0.0f, 0.0f);
  AtomF::PlayBossShotSe();
  // 跳び立つ位置の土煙。
  Particle::Combat::PlayNamedOneShotEffect("atomf_boss_dash_trail.json",
                                           GroundPoint(leapStart_));
}

void CmpAtomFBoss::UpdateLeap(const float dt) {
  (void)dt;
  const float duration = std::max(0.2f, kSlamLeapSeconds);
  const float t = std::clamp(stateElapsed_ / duration, 0.0f, 1.0f);
  const float ease = t * t * (3.0f - 2.0f * t);
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate.x = leapStart_.x + (slamTarget_.x - leapStart_.x) * ease;
  translate.z = leapStart_.z + (slamTarget_.z - leapStart_.z) * ease;
  translate.y = radius_ + kSlamArcHeight * 4.0f * t * (1.0f - t);
  if (t >= 1.0f) {
    translate.y = radius_;
    EndLeap();
  }
}

void CmpAtomFBoss::EndLeap() {
  const Math::Vector3 ground = GroundPoint(RefOwner().GetWorldPosition());
  AtomF::SpawnBossShockwave(ground, kSlamShockRadius, kSlamShockSpeed,
                            kSlamShockBand);
  Particle::Combat::PlayNamedOneShotEffect("atomf_boss_slam.json", ground);
  AtomF::PlayBossDeathBoomSe();
  CombatCameraShake::Instance().RequestCustomShake(0.25f, 0.45f);
  state_ = State::Recover;
  stateElapsed_ = 0.0f;
  recoverOverrideSeconds_ = kSlamRecoverSeconds;
}

void CmpAtomFBoss::ExecuteRing() {
  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  const float offset = Rand01() * kTwoPi;
  const Math::Vector4 color{1.0f, 0.3f, 0.85f, 1.0f};
  for (int i = 0; i < params_.ringBulletCount; ++i) {
    const float angle =
        offset + kTwoPi * static_cast<float>(i) /
                     static_cast<float>(params_.ringBulletCount);
    const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
    AtomF::SpawnEnemyProjectile(origin + dir * (radius_ + 0.1f), dir,
                                params_.ringBulletSpeedMetersPerSecond,
                                params_.ringBulletRadiusMeters, color);
  }
  Particle::Combat::PlayNamedOneShotEffect("atomf_boss_ring_burst.json",
                                           origin);
  AtomF::PlayBossShotSe();
  CombatCameraShake::Instance().RequestCustomShake(0.12f, 0.3f);
}

void CmpAtomFBoss::ExecuteGapRing() {
  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  const Math::Vector4 color{0.45f, 0.85f, 1.0f, 1.0f};
  for (int i = 0; i < kGapRingBulletCount; ++i) {
    const float angle = kTwoPi * static_cast<float>(i) / kGapRingBulletCount;
    // -π〜π に正規化した差で隙間内か判定する。
    float diff = std::fmod(angle - gapAngle_, kTwoPi);
    if (diff > kPi) {
      diff -= kTwoPi;
    } else if (diff < -kPi) {
      diff += kTwoPi;
    }
    if (std::fabs(diff) < kGapRingHalfWidthRadians) {
      continue;
    }
    const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
    AtomF::SpawnEnemyProjectile(origin + dir * (radius_ + 0.1f), dir,
                                kGapRingBulletSpeed,
                                params_.ringBulletRadiusMeters, color);
  }
  Particle::Combat::PlayNamedOneShotEffect("atomf_boss_ring_burst.json", origin);
  AtomF::PlayBossShotSe();
  CombatCameraShake::Instance().RequestCustomShake(0.12f, 0.3f);
}

void CmpAtomFBoss::ExecuteSummon() {
  static int serial = 5000;
  // スキルごとに召喚する種類を変える（散開 / 囲む射手 / 両翼の突進型）。
  std::string nameA = params_.summonVariantA;
  std::string nameB = params_.summonVariantB;
  if (skill_ == Skill::SummonSurround) {
    nameA = "shooter";
    nameB = "sniper";
  } else if (skill_ == Skill::SummonFlank) {
    nameA = "rusher";
    nameB = "dasher";
  }
  const AtomF::EnemyVariantParameters *a = AtomF::FindEnemyVariantByName(nameA);
  const AtomF::EnemyVariantParameters *b = AtomF::FindEnemyVariantByName(nameB);
  AtomF::EnemyVariantParameters fallback{};
  for (std::size_t i = 0; i < summonPoints_.size(); ++i) {
    const AtomF::EnemyVariantParameters *variant = (i % 2 == 0) ? a : b;
    AtomF::EnqueueSceneEntityAdopt(AtomF::CreateEnemyEntity(
        variant != nullptr ? *variant : fallback, summonPoints_[i], ++serial));
    // 出現点に白い閃光の輪。
    Particle::Combat::PlayNamedOneShotEffect("atomf_boss_ring_burst.json",
                                             summonPoints_[i]);
  }
  AtomF::PlayBossShotSe();
  CombatCameraShake::Instance().RequestCustomShake(0.1f, 0.25f);
}

void CmpAtomFBoss::FireVolleyShot() {
  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  Math::Vector3 dir = DirectionToPlayerXZ();
  // 連射の後半ほど少しずつ左右へ散らす。
  const int shotIndex = params_.volleyShotCount - volleyShotsLeft_;
  const float spread = ((shotIndex % 3) - 1) * 0.12f;
  dir = Math::Vector3{dir.x * std::cos(spread) - dir.z * std::sin(spread), 0.0f,
                      dir.x * std::sin(spread) + dir.z * std::cos(spread)};
  AtomF::SpawnEnemyProjectile(origin + dir * (radius_ + 0.1f), dir,
                              params_.volleyBulletSpeedMetersPerSecond,
                              params_.ringBulletRadiusMeters,
                              Math::Vector4{1.0f, 0.4f, 0.2f, 1.0f});
  Particle::Combat::PlayNamedOneShotEffect("atomf_shot_muzzle.json",
                                           origin + dir * (radius_ + 0.1f));
  AtomF::PlayBossShotSe();
}

void CmpAtomFBoss::FireSpiralShot() {
  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  // 対向 2 方向へ同時に撃ち、回転しながら渦を描く。
  for (int arm = 0; arm < 2; ++arm) {
    const float angle = spiralAngle_ + kPi * static_cast<float>(arm);
    const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
    AtomF::SpawnEnemyProjectile(origin + dir * (radius_ + 0.1f), dir,
                                kSpiralBulletSpeed,
                                params_.ringBulletRadiusMeters,
                                Math::Vector4{1.0f, 0.55f, 0.1f, 1.0f});
  }
  spiralAngle_ += kSpiralTurnPerShot;
  if ((spiralShotsLeft_ % 6) == 0) {
    Particle::Combat::PlayNamedOneShotEffect("atomf_boss_spiral_burst.json", origin);
    AtomF::PlayBossShotSe();
    CombatCameraShake::Instance().RequestCustomShake(0.06f, 0.15f);
  }
}

void CmpAtomFBoss::FireFanWave() {
  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  // 波ごとに自機を狙い直す。奇数波は弾数を 1 つ減らして隙間の位置をずらす。
  const Math::Vector3 aim = DirectionToPlayerXZ();
  const float center = std::atan2(aim.z, aim.x);
  const int wave = kFanWaveCount - fanWavesLeft_;
  const int count = (wave % 2 == 0) ? kFanBulletsPerWave : kFanBulletsPerWave - 1;
  for (int i = 0; i < count; ++i) {
    const float t = count > 1 ? static_cast<float>(i) / static_cast<float>(count - 1)
                              : 0.5f;
    const float angle = center + (t * 2.0f - 1.0f) * kFanHalfArcRadians;
    const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
    AtomF::SpawnEnemyProjectile(origin + dir * (radius_ + 0.1f), dir,
                                kFanBulletSpeed, params_.ringBulletRadiusMeters,
                                Math::Vector4{0.7f, 1.0f, 0.2f, 1.0f});
  }
  Particle::Combat::PlayNamedOneShotEffect("atomf_boss_fan_burst.json",
                                           origin + aim * (radius_ + 0.3f));
  AtomF::PlayBossShotSe();
  CombatCameraShake::Instance().RequestCustomShake(0.05f, 0.12f);
}

void CmpAtomFBoss::UpdateEntrance(const float dt) {
  (void)dt;
  const Math::Vector3 ground = GroundPoint(RefOwner().GetWorldPosition());
  if (!entranceEffectPlayed_) {
    entranceEffectPlayed_ = true;
    Particle::Combat::PlayNamedOneShotEffect("atomf_boss_spawn.json", ground);
    AtomF::PlayBossDeathBoomSe();
    CombatCameraShake::Instance().RequestCustomShake(
        0.2f, params_.entranceSeconds * 0.7f);
  }
  const float t = std::clamp(stateElapsed_ / params_.entranceSeconds, 0.0f, 1.0f);
  if (!barGlowPlayed_ && t >= 0.35f) {
    barGlowPlayed_ = true;
    AtomF::PlayBossHpBarGlowSe();
  }
  if (CmpDrawAtomFTelegraph *telegraph = TelegraphOf(RefOwner())) {
    telegraph->Clear();
    // 魔法陣: 外側から収束する 3 重の輪。
    for (int i = 0; i < 3; ++i) {
      const float phase = std::fmod(t * 1.6f + static_cast<float>(i) / 3.0f, 1.0f);
      telegraph->AddRing(ground, radius_ * (4.5f - 3.5f * phase), 0.1f,
                    Math::Vector4{0.8f, 0.3f, 1.0f, 0.75f * (1.0f - t)});
    }
  }
  if (t >= 1.0f) {
    ClearTelegraph();
    if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
      if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
        hit->SetIsActive(true);
      }
    }
    state_ = State::Idle;
    stateElapsed_ = 0.0f;
    idleTimer_ = 1.2f;
  }
}

void CmpAtomFBoss::UpdateIdle(const float dt) {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  if (player != nullptr && player->IsActive()) {
    const Math::Vector3 playerPos = player->GetWorldPosition();
    Math::Vector3 to = playerPos - RefOwner().GetWorldPosition();
    to.y = 0.0f;
    if (to.LengthSquare() > params_.holdDistanceMeters * params_.holdDistanceMeters) {
      MoveToward(playerPos, params_.moveSpeedMetersPerSecond, dt);
    }
  }
  idleTimer_ -= dt;
  if (idleTimer_ <= 0.0f) {
    const bool enraged = enraged_;
    const Skill *sequence = enraged ? kEnragedSkillSequence : kSkillSequence;
    const std::size_t count =
        enraged ? sizeof(kEnragedSkillSequence) / sizeof(kEnragedSkillSequence[0])
                : sizeof(kSkillSequence) / sizeof(kSkillSequence[0]);
    BeginTelegraph(sequence[skillCursor_ % count]);
    ++skillCursor_;
  }
}

void CmpAtomFBoss::FireSweepShot() {
  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  const int fired = kSweepShotCount - sweepShotsLeft_;
  const float t = static_cast<float>(fired) / static_cast<float>(kSweepShotCount - 1);
  const float angle = sweepStartAngle_ + sweepDirection_ * t * 2.0f * kSweepHalfArcRadians;
  const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
  // 弾列を 2 本（速度違い）にして、薙ぎ払いの壁に厚みを持たせる。
  AtomF::SpawnEnemyProjectile(origin + dir * (radius_ + 0.1f), dir,
                              kSweepBulletSpeed, params_.ringBulletRadiusMeters,
                              Math::Vector4{0.9f, 0.3f, 1.0f, 1.0f});
  AtomF::SpawnEnemyProjectile(origin + dir * (radius_ + 0.1f), dir,
                              kSweepBulletSpeed * 0.7f,
                              params_.ringBulletRadiusMeters,
                              Math::Vector4{0.9f, 0.3f, 1.0f, 1.0f});
  if ((sweepShotsLeft_ % 4) == 0) {
    Particle::Combat::PlayNamedOneShotEffect("atomf_shot_muzzle.json",
                                             origin + dir * (radius_ + 0.1f));
    AtomF::PlayBossShotSe();
  }
}

void CmpAtomFBoss::UpdateTelegraph(const float dt) {
  (void)dt;
  float duration = params_.ringTelegraphSeconds;
  switch (skill_) {
  case Skill::Ring: duration = params_.ringTelegraphSeconds; break;
  case Skill::Volley: duration = params_.volleyTelegraphSeconds; break;
  case Skill::Summon:
  case Skill::SummonSurround:
  case Skill::SummonFlank:
    duration = params_.summonTelegraphSeconds;
    break;
  case Skill::Dash: duration = params_.dashTelegraphSeconds; break;
  case Skill::Spiral: duration = kSpiralTelegraphSeconds; break;
  case Skill::GapRing: duration = kGapRingTelegraphSeconds; break;
  case Skill::Sweep: duration = kSweepTelegraphSeconds; break;
  case Skill::Slam: duration = kSlamTelegraphSeconds; break;
  case Skill::Fan: duration = kFanTelegraphSeconds; break;
  }
  duration = std::max(0.1f, duration);
  const float progress = std::clamp(stateElapsed_ / duration, 0.0f, 1.0f);
  const bool locked = stateElapsed_ >= duration - kAimLockSeconds;

  if ((skill_ == Skill::Volley || skill_ == Skill::Dash ||
       skill_ == Skill::Fan) &&
      !locked) {
    lockedDirection_ = DirectionToPlayerXZ();
  }

  if (skill_ == Skill::Slam && !locked) {
    if (Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
        player != nullptr && player->IsActive()) {
      slamTarget_ = GroundPoint(player->GetWorldPosition());
    }
  }

  SetDrawJitter(0.02f + 0.08f * progress, stateElapsed_);

  const Math::Vector3 origin = RefOwner().GetWorldPosition();
  if (CmpDrawAtomFTelegraph *telegraph = TelegraphOf(RefOwner())) {
    telegraph->Clear();
    const float pulse = 0.5f + 0.5f * std::sin(stateElapsed_ * 18.0f);
    switch (skill_) {
    case Skill::Ring: {
      const Math::Vector3 ground = GroundPoint(origin);
      telegraph->AddRing(ground, radius_ + 0.4f + 2.2f * progress, 0.12f,
                    Math::Vector4{1.0f, 0.35f, 0.9f, 0.4f + 0.4f * pulse});
      constexpr int kSpokeCount = 12;
      for (int i = 0; i < kSpokeCount; ++i) {
        const float angle = kTwoPi * static_cast<float>(i) / kSpokeCount;
        const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
        telegraph->AddLine(ground + dir * (radius_ + 0.2f),
                      ground + dir * (radius_ + 1.2f + 2.0f * progress), 0.1f,
                      Math::Vector4{1.0f, 0.3f, 0.7f, 0.5f});
      }
      break;
    }
    case Skill::Volley:
      telegraph->AddLine(origin, origin + lockedDirection_ * 9.0f,
                    locked ? 0.3f : 0.18f,
                    locked ? Math::Vector4{1.0f, 0.1f, 0.1f, 0.7f}
                           : Math::Vector4{1.0f, 0.55f, 0.2f, 0.35f});
      break;
    case Skill::Fan: {
      // 扇の両端と中央を黄緑の線で示す。確定後は中央線が太くなる。
      const Math::Vector3 ground = GroundPoint(origin);
      const float center = std::atan2(lockedDirection_.z, lockedDirection_.x);
      for (int side = -1; side <= 1; ++side) {
        const float angle = center + static_cast<float>(side) * kFanHalfArcRadians;
        const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
        const bool mid = side == 0;
        telegraph->AddLine(ground + dir * (radius_ + 0.2f),
                      ground + dir * (mid ? 10.0f : 8.0f),
                      mid ? (locked ? 0.28f : 0.14f) : 0.1f,
                      Math::Vector4{0.7f, 1.0f, 0.2f,
                                    locked ? 0.7f : 0.25f + 0.2f * pulse});
      }
      telegraph->AddRing(ground, radius_ + 0.3f + 1.0f * progress, 0.1f,
                    Math::Vector4{0.7f, 1.0f, 0.2f, 0.35f + 0.3f * pulse});
      break;
    }
    case Skill::Spiral: {
      // 回り始める 3 本の腕を予告する（実行時と同じ向きから）。
      const Math::Vector3 ground = GroundPoint(origin);
      for (int arm = 0; arm < 3; ++arm) {
        const float angle = spiralAngle_ + stateElapsed_ * 3.0f +
                            kTwoPi * static_cast<float>(arm) / 3.0f;
        const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
        telegraph->AddLine(ground + dir * (radius_ + 0.2f),
                      ground + dir * (radius_ + 1.5f + 3.0f * progress), 0.12f,
                      Math::Vector4{1.0f, 0.45f, 0.1f, 0.4f + 0.4f * pulse});
      }
      telegraph->AddRing(ground, radius_ + 0.3f + 1.2f * progress, 0.1f,
                    Math::Vector4{1.0f, 0.5f, 0.15f, 0.35f + 0.35f * pulse});
      break;
    }
    case Skill::Sweep: {
      // 扇の両端を淡い紫の線で、振り始める端を強い線で示す。
      const Math::Vector3 ground = GroundPoint(origin);
      const float endAngle =
          sweepStartAngle_ + sweepDirection_ * 2.0f * kSweepHalfArcRadians;
      const Math::Vector3 startDir{std::cos(sweepStartAngle_), 0.0f,
                                   std::sin(sweepStartAngle_)};
      const Math::Vector3 endDir{std::cos(endAngle), 0.0f, std::sin(endAngle)};
      telegraph->AddLine(ground + startDir * (radius_ + 0.2f),
                    ground + startDir * 9.0f, 0.22f,
                    Math::Vector4{1.0f, 0.2f, 0.9f, 0.3f + 0.4f * progress});
      telegraph->AddLine(ground + endDir * (radius_ + 0.2f),
                    ground + endDir * 9.0f, 0.12f,
                    Math::Vector4{0.8f, 0.4f, 1.0f, 0.25f});
      constexpr int kArcSegments = 8;
      for (int i = 0; i < kArcSegments; ++i) {
        const float a0 = sweepStartAngle_ + sweepDirection_ * 2.0f *
                                                kSweepHalfArcRadians *
                                                static_cast<float>(i) / kArcSegments;
        const float a1 = sweepStartAngle_ + sweepDirection_ * 2.0f *
                                                kSweepHalfArcRadians *
                                                static_cast<float>(i + 1) / kArcSegments;
        const float reach = radius_ + 3.0f + 3.0f * progress;
        telegraph->AddLine(
            ground + Math::Vector3{std::cos(a0), 0.0f, std::sin(a0)} * reach,
            ground + Math::Vector3{std::cos(a1), 0.0f, std::sin(a1)} * reach,
            0.1f, Math::Vector4{0.9f, 0.3f, 1.0f, 0.3f + 0.3f * pulse});
      }
      break;
    }
    case Skill::GapRing: {
      // 弾の出る方向は水色の線、空く隙間は緑の太い線で示す。
      const Math::Vector3 ground = GroundPoint(origin);
      const float reach = radius_ + 1.0f + 3.0f * progress;
      for (int i = 0; i < kGapRingBulletCount; i += 2) {
        const float angle = kTwoPi * static_cast<float>(i) / kGapRingBulletCount;
        float diff = std::fmod(angle - gapAngle_, kTwoPi);
        if (diff > kPi) {
          diff -= kTwoPi;
        } else if (diff < -kPi) {
          diff += kTwoPi;
        }
        if (std::fabs(diff) < kGapRingHalfWidthRadians) {
          continue;
        }
        const Math::Vector3 dir{std::cos(angle), 0.0f, std::sin(angle)};
        telegraph->AddLine(ground + dir * (radius_ + 0.2f), ground + dir * reach,
                      0.1f, Math::Vector4{0.4f, 0.8f, 1.0f, 0.35f + 0.3f * pulse});
      }
      const Math::Vector3 gapDir{std::cos(gapAngle_), 0.0f, std::sin(gapAngle_)};
      telegraph->AddLine(ground + gapDir * (radius_ + 0.2f),
                    ground + gapDir * (radius_ + 5.0f), 0.5f,
                    Math::Vector4{0.3f, 1.0f, 0.5f, 0.25f + 0.3f * progress});
      telegraph->AddRing(ground, radius_ + 0.4f + 1.8f * progress, 0.1f,
                    Math::Vector4{0.4f, 0.8f, 1.0f, 0.4f + 0.3f * pulse});
      break;
    }
    case Skill::Summon:
    case Skill::SummonSurround:
    case Skill::SummonFlank: {
      // 種類ごとに色を変える（散開=水色 / 囲む=黄 / 両翼=赤橙）。
      const Math::Vector4 tint =
          skill_ == Skill::SummonSurround
              ? Math::Vector4{1.0f, 0.9f, 0.3f, 1.0f}
              : (skill_ == Skill::SummonFlank
                     ? Math::Vector4{1.0f, 0.45f, 0.2f, 1.0f}
                     : Math::Vector4{0.5f, 0.9f, 1.0f, 1.0f});
      const Math::Vector3 ground = GroundPoint(origin);
      for (const Math::Vector3 &point : summonPoints_) {
        telegraph->AddRing(point, 0.9f - 0.5f * progress, 0.08f,
                      Math::Vector4{tint.x, tint.y, tint.z, 0.5f + 0.4f * pulse});
        telegraph->AddRing(point, 0.3f, 0.06f,
                      Math::Vector4{tint.x, tint.y, tint.z, 0.8f});
        // ボスから出現点へ伸びる細い線で「どこへ呼ぶか」を示す。
        telegraph->AddLine(ground, point, 0.05f,
                      Math::Vector4{tint.x, tint.y, tint.z, 0.15f + 0.25f * progress});
      }
      break;
    }
    case Skill::Slam: {
      // 着地点を縮む輪と十字で示す。確定後は赤く太くなる。
      const Math::Vector4 tone = locked ? Math::Vector4{1.0f, 0.1f, 0.05f, 0.75f}
                                        : Math::Vector4{1.0f, 0.55f, 0.15f, 0.4f};
      telegraph->AddRing(slamTarget_, kSlamTargetRadius, 0.14f, tone);
      telegraph->AddRing(slamTarget_,
                    kSlamTargetRadius * (1.0f - 0.7f * progress), 0.1f,
                    Math::Vector4{1.0f, 0.3f, 0.1f, 0.3f + 0.4f * pulse});
      telegraph->AddRing(slamTarget_, kSlamShockRadius, 0.06f,
                    Math::Vector4{1.0f, 0.4f, 0.2f, 0.18f});
      telegraph->AddLine(slamTarget_ + Math::Vector3{-kSlamTargetRadius, 0.0f, 0.0f},
                    slamTarget_ + Math::Vector3{kSlamTargetRadius, 0.0f, 0.0f},
                    0.08f, tone);
      telegraph->AddLine(slamTarget_ + Math::Vector3{0.0f, 0.0f, -kSlamTargetRadius},
                    slamTarget_ + Math::Vector3{0.0f, 0.0f, kSlamTargetRadius},
                    0.08f, tone);
      break;
    }
    case Skill::Dash:
      telegraph->AddLine(origin, origin + lockedDirection_ * 14.0f,
                    radius_ * 1.7f,
                    locked ? Math::Vector4{1.0f, 0.1f, 0.05f, 0.5f}
                           : Math::Vector4{1.0f, 0.5f, 0.15f, 0.22f});
      break;
    }
  }

  if (stateElapsed_ < duration) {
    return;
  }

  ClearTelegraph();
  SetDrawJitter(0.0f, 0.0f);
  switch (skill_) {
  case Skill::Ring:
    ExecuteRing();
    FinishSkill();
    break;
  case Skill::GapRing:
    ExecuteGapRing();
    FinishSkill();
    break;
  case Skill::Summon:
  case Skill::SummonSurround:
  case Skill::SummonFlank:
    ExecuteSummon();
    FinishSkill();
    break;
  case Skill::Volley:
    state_ = State::Execute;
    stateElapsed_ = 0.0f;
    volleyShotsLeft_ = params_.volleyShotCount;
    volleyTimer_ = 0.0f;
    break;
  case Skill::Dash:
    BeginDash();
    break;
  case Skill::Slam:
    BeginLeap();
    break;
  case Skill::Fan:
    state_ = State::Execute;
    stateElapsed_ = 0.0f;
    fanWavesLeft_ = kFanWaveCount;
    volleyTimer_ = 0.0f;
    break;
  case Skill::Spiral:
    state_ = State::Execute;
    stateElapsed_ = 0.0f;
    spiralShotsLeft_ = kSpiralShotCount;
    volleyTimer_ = 0.0f;
    break;
  case Skill::Sweep:
    state_ = State::Execute;
    stateElapsed_ = 0.0f;
    sweepShotsLeft_ = kSweepShotCount;
    volleyTimer_ = 0.0f;
    break;
  }
}

void CmpAtomFBoss::UpdateExecute(const float dt) {
  if (skill_ == Skill::Spiral) {
    volleyTimer_ -= dt;
    if (volleyTimer_ > 0.0f) {
      return;
    }
    FireSpiralShot();
    --spiralShotsLeft_;
    volleyTimer_ = kSpiralIntervalSeconds;
    if (spiralShotsLeft_ <= 0) {
      FinishSkill();
    }
    return;
  }
  if (skill_ == Skill::Fan) {
    volleyTimer_ -= dt;
    if (volleyTimer_ > 0.0f) {
      return;
    }
    FireFanWave();
    --fanWavesLeft_;
    volleyTimer_ = kFanWaveIntervalSeconds;
    if (fanWavesLeft_ <= 0) {
      FinishSkill();
    }
    return;
  }
  if (skill_ == Skill::Sweep) {
    volleyTimer_ -= dt;
    if (volleyTimer_ > 0.0f) {
      return;
    }
    FireSweepShot();
    --sweepShotsLeft_;
    volleyTimer_ = kSweepIntervalSeconds;
    if (sweepShotsLeft_ <= 0) {
      FinishSkill();
    }
    return;
  }
  // Execute に長く留まるのは連射・スパイラル・薙ぎ払い。
  volleyTimer_ -= dt;
  if (volleyTimer_ > 0.0f) {
    return;
  }
  FireVolleyShot();
  --volleyShotsLeft_;
  volleyTimer_ = params_.volleyIntervalSeconds;
  if (volleyShotsLeft_ <= 0) {
    FinishSkill();
  }
}

void CmpAtomFBoss::UpdateDash(const float dt) {
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate += lockedDirection_ * (params_.dashSpeedMetersPerSecond * dt);
  translate.y = radius_;

  // 軌跡の残光。
  if (std::fmod(stateElapsed_, 0.08f) < dt) {
    Particle::Combat::PlayNamedOneShotEffect("atomf_boss_dash_trail.json",
                                             GroundPoint(translate));
  }

  const bool touchedWall =
      AtomF::IsWorldXZTouchingArenaWall(translate.x, translate.z, radius_);
  if (touchedWall || stateElapsed_ >= params_.dashMaxSeconds) {
    EndDash();
  }
}

void CmpAtomFBoss::UpdateRecover(const float dt) {
  (void)dt;
  const float recoverSeconds =
      recoverOverrideSeconds_ >= 0.0f ? recoverOverrideSeconds_ : params_.recoverSeconds;
  if (stateElapsed_ >= recoverSeconds) {
    recoverOverrideSeconds_ = -1.0f;
    FinishSkill();
  }
}

void CmpAtomFBoss::DestroyHostileEntitiesOnDeath() {
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  // 走査中に登録が変わらないよう、先に対象を集める。
  std::vector<Entity *> owners;
  for (CmpHitSphere *hit : judge->RegisteredHitSpheres()) {
    if (hit == nullptr) {
      continue;
    }
    Entity *owner = &hit->RefOwner();
    if (owner != &RefOwner()) {
      owners.push_back(owner);
    }
  }
  for (Entity *owner : owners) {
    if (tags.EntityHasRole(*owner, "enemyWeapon")) {
      owner->RequestDestroy();
    } else if (tags.EntityHasRole(*owner, "enemy") &&
               owner->HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
      if (CmpAtomFEnemyChainExplosion *chain =
              owner->GetUpdCmp<CmpAtomFEnemyChainExplosion>()) {
        chain->BeginDyingSilently();
      }
    }
  }
}

void CmpAtomFBoss::BeginDying() {
  state_ = State::Dying;
  stateElapsed_ = 0.0f;
  deathEffectTimer_ = 0.0f;
  deathBurstCount_ = 0;
  RefOwner().RefTransform().translate.y = radius_;
  recoverOverrideSeconds_ = -1.0f;
  deathAnchor_ = RefOwner().RefTransform().translate;
  ClearTelegraph();
  SetDrawJitter(0.0f, 0.0f);
  if (RefOwner().HasUpdCmp<CmpHitSphere>()) {
    if (CmpHitSphere *hit = RefOwner().GetUpdCmp<CmpHitSphere>()) {
      hit->SetIsActive(false);
    }
  }
  DestroyHostileEntitiesOnDeath();
  AtomF::PlayBossDeathBoomSe();
  CombatCameraShake::Instance().RequestCustomShake(0.3f, 0.8f);
}

void CmpAtomFBoss::UpdateDying(const float dt) {
  const float duration = std::max(0.5f, params_.deathSeconds);
  const float t = std::clamp(stateElapsed_ / duration, 0.0f, 1.0f);

  // 震えながら位置を保つ（吹き飛ばさない）。
  RefOwner().RefTransform().translate = deathAnchor_;
  SetDrawJitter(0.05f + 0.25f * t, stateElapsed_);

  // 終盤ほど間隔が詰まる連続爆発。
  deathEffectTimer_ -= dt;
  if (deathEffectTimer_ <= 0.0f && t < 1.0f) {
    deathEffectTimer_ = 0.38f - 0.28f * t;
    const float angle = Rand01() * kTwoPi;
    const float dist = Rand01() * radius_ * 0.9f;
    const Math::Vector3 point = deathAnchor_ + Math::Vector3{std::cos(angle) * dist,
                                                             (Rand01() - 0.5f) * radius_,
                                                             std::sin(angle) * dist};
    static uint32_t sequence = 200000U;
    Particle::Combat::PlayBossDeathExplosionEffect(point, ++sequence);
    CombatCameraShake::Instance().RequestCustomShake(0.12f + 0.2f * t, 0.3f);
    if ((++deathBurstCount_ % 3) == 0) {
      AtomF::PlayBossDeathBoomSe();
    }
  }

  if (CmpDrawAtomFTelegraph *telegraph = TelegraphOf(RefOwner())) {
    telegraph->Clear();
    // 崩壊前に広がる輪。
    telegraph->AddRing(GroundPoint(deathAnchor_), radius_ * (1.0f + 3.0f * t), 0.12f,
                  Math::Vector4{1.0f, 0.7f, 0.3f, 0.6f * (1.0f - t)});
  }

  if (stateElapsed_ >= duration) {
    // フィナーレ: 大爆発して消える。
    ClearTelegraph();
    static uint32_t finaleSequence = 300000U;
    for (int i = 0; i < 6; ++i) {
      const float angle = kTwoPi * static_cast<float>(i) / 6.0f;
      Particle::Combat::PlayBossDeathExplosionEffect(
          deathAnchor_ + Math::Vector3{std::cos(angle) * radius_ * 0.7f, 0.0f,
                                       std::sin(angle) * radius_ * 0.7f},
          ++finaleSequence);
    }
    Particle::Combat::PlayNamedOneShotEffect("atomf_boss_finale.json",
                                             deathAnchor_);
    AtomF::PlayBossDeathBoomSe();
    CombatCameraShake::Instance().RequestCustomShake(0.45f, 0.9f);
    RefOwner().Finalize();
    RefOwner().RequestDestroy();
  }
}

void CmpAtomFBoss::ApplyVisual(const float dt) {
  CmpMeshSphere *mesh = MeshOf(RefOwner());
  if (mesh == nullptr) {
    return;
  }

  if (hitFlashSeconds_ > 0.0f) {
    hitFlashSeconds_ = std::max(0.0f, hitFlashSeconds_ - dt);
  }
  const float healthRatio =
      static_cast<float>(health_) / static_cast<float>(params_.maxHealth);
  displayedHealthRatio_ =
      std::max(healthRatio, displayedHealthRatio_ - 0.5f * dt);

  Math::Vector4 base{params_.colorR, params_.colorG, params_.colorB, 1.0f};
  if (enraged_) {
    base = Lerp(base, Math::Vector4{1.0f, 0.15f, 0.2f, 1.0f}, 0.45f);
  }

  Math::Vector4 color = base;
  float meshRadius = radius_;

  switch (state_) {
  case State::Entrance: {
    const float t = std::clamp(stateElapsed_ / params_.entranceSeconds, 0.0f, 1.0f);
    const float grow = std::clamp((t - 0.25f) / 0.75f, 0.0f, 1.0f);
    meshRadius = radius_ * std::max(0.0f, EaseOutBack(grow));
    color.w = 0.35f + 0.65f * grow;
    break;
  }
  case State::Telegraph: {
    const float pulse = 0.5f + 0.5f * std::sin(stateElapsed_ * 16.0f);
    color = Lerp(base, Math::Vector4{1.0f, 0.35f, 0.15f, 1.0f}, 0.3f + 0.4f * pulse);
    break;
  }
  case State::Recover: {
    const float pulse = 0.5f + 0.5f * std::sin(stateElapsed_ * 9.0f);
    color = Lerp(base * 0.5f, Math::Vector4{0.9f, 0.9f, 1.0f, 1.0f}, 0.25f * pulse);
    color.w = 1.0f;
    break;
  }
  case State::Dying: {
    const float t = std::clamp(stateElapsed_ / std::max(0.5f, params_.deathSeconds), 0.0f, 1.0f);
    const float flicker = 0.5f + 0.5f * std::sin(stateElapsed_ * (14.0f + 30.0f * t));
    color = Lerp(base, Math::Vector4{1.0f, 0.95f, 0.8f, 1.0f}, flicker);
    meshRadius = radius_ * (1.0f + 0.25f * t * t);
    break;
  }
  default:
    break;
  }

  if (hitFlashSeconds_ > 0.0f) {
    color = Lerp(color, Math::Vector4{1.0f, 1.0f, 1.0f, 1.0f},
                 hitFlashSeconds_ / kBossHitFlashSeconds);
  }
  color.w = std::min(color.w, 1.0f);
  mesh->SetColor(color);
  mesh->SetRadius(meshRadius);
}

void CmpAtomFBoss::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  // シーンへ採用されて初めて「現在のボス」になる（採用待ちのまま破棄されても残らない）。
  g_currentBoss = this;
  const float dt = frameDeltaTime_->GetDeltaTime();
  stateElapsed_ += dt;

  switch (state_) {
  case State::Entrance: UpdateEntrance(dt); break;
  case State::Idle: UpdateIdle(dt); break;
  case State::Telegraph: UpdateTelegraph(dt); break;
  case State::Execute: UpdateExecute(dt); break;
  case State::Dash: UpdateDash(dt); break;
  case State::Leap: UpdateLeap(dt); break;
  case State::Recover: UpdateRecover(dt); break;
  case State::Dying: UpdateDying(dt); break;
  }

  // 撃破演出の終了で自身が破棄された後は描画更新しない。
  if (state_ == State::Dying && !RefOwner().IsActive()) {
    return;
  }
  ApplyVisual(dt);
}

} // namespace Atrum
