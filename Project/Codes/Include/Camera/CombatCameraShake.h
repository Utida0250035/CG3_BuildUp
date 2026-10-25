#pragma once

#include "Math/Vector3.h"

namespace Atrum {

class Camera;

/// 戦闘中の被弾カメラシェイク（ステージ戦闘カメラ pose にオフセットを載せる）
class CombatCameraShake {
public:
  static CombatCameraShake &Instance();

  void Reset();

  void RequestBossHitShake();
  void RequestPlayerHitShake();

  void Update(float deltaTimeSeconds);
  void ApplyStageBattleView(Camera *camera);

private:
  void Trigger(float amplitude, float durationSeconds);
  Math::Vector3 SampleOffset() const;

  float timer_ = 0.0f;
  float duration_ = 0.0f;
  float amplitude_ = 0.0f;
};

} // namespace Atrum
