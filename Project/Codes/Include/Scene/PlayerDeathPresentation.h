#pragma once

#include "Math/Vector3.h"
#include <cstdint>
#include <vector>

namespace Atrum {

class Entity;

/// 自機撃墜時：ダメージパーティクルを散らしつつ本体を縮小する
class PlayerDeathPresentation {
public:
  void Begin(Entity *player);
  void Update(float deltaTimeSeconds);

  bool IsFinished() const { return isFinished_; }

private:
  struct AttachedEntityState {
    Entity *entity = nullptr;
    Math::Vector3 initialScale{1.0f, 1.0f, 1.0f};
  };

  struct ScheduledHitEffect {
    Entity *attachedEntity = nullptr;
    Math::Vector3 localOffset{};
    float fireAtSeconds = 0.0f;
  };

  Entity *player_ = nullptr;
  Math::Vector3 initialPlayerScale_{1.0f, 1.0f, 1.0f};
  std::vector<AttachedEntityState> attachedEntities_{};
  float elapsedSeconds_ = 0.0f;
  size_t nextEffectIndex_ = 0;
  uint32_t effectSerial_ = 0;
  bool isFinished_ = false;

  std::vector<ScheduledHitEffect> scheduledHitEffects_{};

  static constexpr int kHitEffectCount = 10;
  static constexpr float kHitEffectIntervalSeconds = 0.18f;
  static constexpr float kShrinkDurationSeconds = 2.2f;
  static constexpr float kFinishTailSeconds = 0.45f;
  static constexpr float kHitEffectLocalRadius = 0.42f;
  static constexpr int kHitEffectCountPerAttachedEntity = 3;
  static constexpr float kAttachedHitEffectLocalRadius = 0.28f;

  void CollectAttachedEntities();
  void DisablePlayerCombat();
  void ApplyShrinkScale(float scaleFactor);
  void FireScheduledHitEffect(const ScheduledHitEffect &effect);
  static Math::Vector3 SampleHitEffectLocalOffset(int index, int count,
                                                  float radius);
};

} // namespace Atrum
