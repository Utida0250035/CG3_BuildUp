#pragma once

#include "Math/Vector3.h"
#include <cstdint>
#include <vector>

namespace Atrum {

class Entity;

/// ボス撃破後：複数箇所の爆発パーティクルを時間差で出しつつ本体を縮小する
class BossDeathPresentation {
public:
  void Begin(Entity *boss);
  void Update(float deltaTimeSeconds);

  bool IsFinished() const { return isFinished_; }

private:
  struct ScheduledExplosion {
    Math::Vector3 localOffset{};
    float fireAtSeconds = 0.0f;
  };

  Entity *boss_ = nullptr;
  Math::Vector3 initialBossScale_{1.0f, 1.0f, 1.0f};
  float elapsedSeconds_ = 0.0f;
  size_t nextExplosionIndex_ = 0;
  uint32_t explosionEffectSerial_ = 0;
  bool isFinished_ = false;

  std::vector<ScheduledExplosion> scheduledExplosions_{};

  static constexpr int kExplosionCount = 12;
  static constexpr float kExplosionIntervalSeconds = 0.22f;
  static constexpr float kShrinkDurationSeconds = 3.0f;
  static constexpr float kFinishTailSeconds = 0.55f;
  static constexpr float kExplosionLocalRadius = 0.92f;

  void FireExplosionAtLocalOffset(const Math::Vector3 &localOffset);
  static Math::Vector3 SampleExplosionLocalOffset(int index, int count,
                                                  float radius);
};

} // namespace Atrum
