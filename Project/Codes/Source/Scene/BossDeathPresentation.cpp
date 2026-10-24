#include "Scene/BossDeathPresentation.h"

#include "Math/Easing.h"
#include "Math/UniformSphereDistribution.h"
#include "System/BossCombatCleanup.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <algorithm>

namespace Atrum {

Math::Vector3 BossDeathPresentation::SampleExplosionLocalOffset(
    const int index, const int count, const float radius) {
  return Math::SampleUniformSpherePoint(index, count, radius);
}

void BossDeathPresentation::FireExplosionAtLocalOffset(
    const Math::Vector3 &localOffset) {
  if (boss_ == nullptr) {
    return;
  }

  const Math::Vector3 worldPosition =
      boss_->TransformLocalPointToWorld(localOffset);
  Particle::Combat::PlayBossDeathExplosionEffect(worldPosition,
                                                 explosionEffectSerial_++);
}

void BossDeathPresentation::Begin(Entity *boss) {
  boss_ = boss;
  elapsedSeconds_ = 0.0f;
  nextExplosionIndex_ = 0;
  explosionEffectSerial_ = 0;
  isFinished_ = false;
  scheduledExplosions_.clear();

  if (boss_ == nullptr) {
    isFinished_ = true;
    return;
  }

  initialBossScale_ = boss_->GetTransform().scale;
  ClearBossAttackTelegraphAndBullets(*boss_);

  scheduledExplosions_.reserve(kExplosionCount);
  for (int index = 0; index < kExplosionCount; ++index) {
    ScheduledExplosion explosion{};
    explosion.localOffset =
        SampleExplosionLocalOffset(index, kExplosionCount, kExplosionLocalRadius);
    explosion.fireAtSeconds =
        static_cast<float>(index) * kExplosionIntervalSeconds;
    scheduledExplosions_.push_back(explosion);
  }
}

void BossDeathPresentation::Update(const float deltaTimeSeconds) {
  if (isFinished_ || boss_ == nullptr) {
    return;
  }

  elapsedSeconds_ += deltaTimeSeconds;

  while (nextExplosionIndex_ < scheduledExplosions_.size() &&
         scheduledExplosions_[nextExplosionIndex_].fireAtSeconds <=
             elapsedSeconds_) {
    FireExplosionAtLocalOffset(
        scheduledExplosions_[nextExplosionIndex_].localOffset);
    ++nextExplosionIndex_;
  }

  const float shrinkRatio =
      std::clamp(elapsedSeconds_ / kShrinkDurationSeconds, 0.0f, 1.0f);
  const float scaleFactor = 1.0f - Math::Ease::InQuart(shrinkRatio);
  boss_->RefTransform().scale = initialBossScale_ * scaleFactor;

  const float finishAtSeconds =
      kShrinkDurationSeconds + kFinishTailSeconds;
  if (elapsedSeconds_ >= finishAtSeconds &&
      nextExplosionIndex_ >= scheduledExplosions_.size()) {
    boss_->RefTransform().scale = initialBossScale_ * 0.0f;
    isFinished_ = true;
  }
}

} // namespace Atrum
