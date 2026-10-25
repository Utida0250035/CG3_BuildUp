#include "Scene/PlayerDeathPresentation.h"

#include "Math/Easing.h"
#include "Math/UniformSphereDistribution.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"
#include "System/Playable/CmpDrawPlayerParryField.h"
#include "System/PlaySe.h"

#include <algorithm>
#include <unordered_set>

namespace Atrum {

namespace {

void PlayPlayerDeathBoomSe() {
  const std::string sePath = PlaySeDetail::ResolveExistingSePath(
      {"./Assets/Audios/sePlayerDeathBoom.mp3",
       "./Resources/sePlayerDeathBoom.mp3"});
  if (sePath.empty()) {
    return;
  }

  PlaySe(sePath);
}

void CollectDescendantEntities(Entity *root,
                               std::vector<Entity *> &outEntities,
                               std::unordered_set<Entity *> &visited) {
  if (root == nullptr) {
    return;
  }

  for (Entity *child : root->GetChilds()) {
    if (child == nullptr || visited.contains(child)) {
      continue;
    }

    visited.insert(child);
    outEntities.push_back(child);
    CollectDescendantEntities(child, outEntities, visited);
  }
}

} // namespace

Math::Vector3 PlayerDeathPresentation::SampleHitEffectLocalOffset(
    const int index, const int count, const float radius) {
  return Math::SampleUniformSpherePoint(index, count, radius);
}

void PlayerDeathPresentation::CollectAttachedEntities() {
  attachedEntities_.clear();
  if (player_ == nullptr) {
    return;
  }

  std::vector<Entity *> descendants{};
  std::unordered_set<Entity *> visited{};
  CollectDescendantEntities(player_, descendants, visited);

  attachedEntities_.reserve(descendants.size());
  for (Entity *entity : descendants) {
    AttachedEntityState state{};
    state.entity = entity;
    state.initialScale = entity->GetTransform().scale;
    attachedEntities_.push_back(state);
  }
}

void PlayerDeathPresentation::DisablePlayerCombat() {
  if (player_ == nullptr) {
    return;
  }

  if (player_->HasUpdCmp<CmpHitSphere>()) {
    player_->GetUpdCmp<CmpHitSphere>()->SetIsActive(false);
  }

  if (player_->HasDrawCmp<Player::CmpDrawPlayerParryField>()) {
    player_->GetDrawCmp<Player::CmpDrawPlayerParryField>()->SetVisible(false);
  }

  for (const AttachedEntityState &attached : attachedEntities_) {
    if (attached.entity == nullptr) {
      continue;
    }

    if (attached.entity->HasUpdCmp<CmpHitSphere>()) {
      attached.entity->GetUpdCmp<CmpHitSphere>()->SetIsActive(false);
    }
  }
}

void PlayerDeathPresentation::FireScheduledHitEffect(
    const ScheduledHitEffect &effect) {
  Math::Vector3 worldPosition{};
  if (effect.attachedEntity != nullptr) {
    worldPosition =
        effect.attachedEntity->TransformLocalPointToWorld(effect.localOffset);
  } else if (player_ != nullptr) {
    worldPosition = player_->TransformLocalPointToWorld(effect.localOffset);
  } else {
    return;
  }

  Particle::Combat::PlayPlayerDeathDamageScatterEffect(worldPosition,
                                                       effectSerial_++);
  PlayPlayerDeathBoomSe();
}

void PlayerDeathPresentation::ApplyShrinkScale(const float scaleFactor) {
  if (player_ == nullptr) {
    return;
  }

  player_->RefTransform().scale = initialPlayerScale_ * scaleFactor;

  for (const AttachedEntityState &attached : attachedEntities_) {
    if (attached.entity == nullptr) {
      continue;
    }

    attached.entity->RefTransform().scale = attached.initialScale * scaleFactor;
  }
}

void PlayerDeathPresentation::Begin(Entity *player) {
  player_ = player;
  elapsedSeconds_ = 0.0f;
  nextEffectIndex_ = 0;
  effectSerial_ = 0;
  isFinished_ = false;
  scheduledHitEffects_.clear();
  attachedEntities_.clear();

  if (player_ == nullptr) {
    isFinished_ = true;
    return;
  }

  initialPlayerScale_ = player_->GetTransform().scale;
  CollectAttachedEntities();
  DisablePlayerCombat();

  scheduledHitEffects_.reserve(
      kHitEffectCount +
      attachedEntities_.size() * kHitEffectCountPerAttachedEntity);
  for (int index = 0; index < kHitEffectCount; ++index) {
    ScheduledHitEffect effect{};
    effect.localOffset =
        SampleHitEffectLocalOffset(index, kHitEffectCount, kHitEffectLocalRadius);
    effect.fireAtSeconds =
        static_cast<float>(index) * kHitEffectIntervalSeconds;
    scheduledHitEffects_.push_back(effect);
  }

  int attachedEffectSerial = 0;
  for (const AttachedEntityState &attached : attachedEntities_) {
    if (attached.entity == nullptr) {
      continue;
    }

    for (int index = 0; index < kHitEffectCountPerAttachedEntity; ++index) {
      ScheduledHitEffect effect{};
      effect.attachedEntity = attached.entity;
      effect.localOffset = SampleHitEffectLocalOffset(
          attachedEffectSerial, kHitEffectCountPerAttachedEntity,
          kAttachedHitEffectLocalRadius);
      effect.fireAtSeconds =
          static_cast<float>(attachedEffectSerial) * kHitEffectIntervalSeconds;
      scheduledHitEffects_.push_back(effect);
      ++attachedEffectSerial;
    }
  }

  std::sort(scheduledHitEffects_.begin(), scheduledHitEffects_.end(),
            [](const ScheduledHitEffect &a, const ScheduledHitEffect &b) {
              return a.fireAtSeconds < b.fireAtSeconds;
            });
}

void PlayerDeathPresentation::Update(const float deltaTimeSeconds) {
  if (isFinished_ || player_ == nullptr) {
    return;
  }

  elapsedSeconds_ += deltaTimeSeconds;

  while (nextEffectIndex_ < scheduledHitEffects_.size() &&
         scheduledHitEffects_[nextEffectIndex_].fireAtSeconds <=
             elapsedSeconds_) {
    FireScheduledHitEffect(scheduledHitEffects_[nextEffectIndex_]);
    ++nextEffectIndex_;
  }

  const float shrinkRatio =
      std::clamp(elapsedSeconds_ / kShrinkDurationSeconds, 0.0f, 1.0f);
  const float scaleFactor = 1.0f - Math::Ease::InQuart(shrinkRatio);
  ApplyShrinkScale(scaleFactor);

  const float finishAtSeconds = kShrinkDurationSeconds + kFinishTailSeconds;
  if (elapsedSeconds_ >= finishAtSeconds &&
      nextEffectIndex_ >= scheduledHitEffects_.size()) {
    ApplyShrinkScale(0.0f);
    isFinished_ = true;
  }
}

} // namespace Atrum
