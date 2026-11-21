#pragma once

#include <algorithm>

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class CmpChildEntitys;
class CmpDirection;
class CmpLhRigidBody;
class FrameDeltaTime;

namespace Player {

/// 全方位 STG 自機：移動（WASD＋L）、エイム（マウス＋R）、単発射撃（Space／A）とクールダウン
class CmpStgPlayerPilot final : public ComponentUpd {
private:
  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpLhRigidBody *cmpLhRigidBody_ = nullptr;
  CmpDirection *cmpDirection_ = nullptr;
  CmpChildEntitys *cmpChildEntitys_ = nullptr;

  float moveSpeed_ = 8.0f;
  float shootCooldownSeconds_ = 0.2f;
  float bulletSpeed_ = 14.0f;
  float bulletSpawnOffset_ = 0.55f;

  float shootCooldownTimer_ = 0.0f;
  Math::Vector3 aimDirection_{1.0f, 0.0f, 0.0f};

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::INPUT; }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;

  void SetMoveSpeed(const float speed) { moveSpeed_ = speed; }
  void SetShootCooldownSeconds(const float seconds) {
    shootCooldownSeconds_ = std::max(0.0f, seconds);
  }
  void SetBulletSpeed(const float speed) { bulletSpeed_ = speed; }
  void SetBulletSpawnOffset(const float offset) { bulletSpawnOffset_ = offset; }
};

} // namespace Player
} // namespace Atrum