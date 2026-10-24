#pragma once

#include "System/Entity/ComponentUpd.h"
#include "System/Entity/EntityType.h"
#include <cstdint>
#include <set>
#include <vector>

namespace Atrum {

class CmpHitSphere;
class FrameDeltaTime;

class CmpHealthHit : public ComponentUpd {
private:
  uint32_t healthCount_ = 0;
  uint32_t initialHealthCount_ = 1;
  std::set<EntityType> hurterTypes_{};
  bool isHurt_ = false;

  CmpHitSphere *cmpHitSphere_ = nullptr;
  FrameDeltaTime *frameDeltaTime_ = nullptr;

  float invincibleTimer_ = 0.0f;
  float invincibleTime_ = 0.25f;

public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::HIT_RESPONSE;
  }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;

  /// <summary>
  /// 攻撃元エンティティの種別が HurterTypes に含まれるときダメージを1与える（無敵中は false）
  /// </summary>
  bool TryTakeDamageFrom(const Entity *attacker);

  void SetHurterTypes(const std::vector<EntityType> &hurterTypes) {
    for (const auto &type : hurterTypes) {
      hurterTypes_.emplace(type);
    }
  };

  bool GetIsHurt() const { return isHurt_; }

  uint32_t GetInitialHealthCount() const { return initialHealthCount_; }

  uint32_t GetHealthCount() const { return healthCount_; }

  void SetInitialHealthCount(const uint32_t initialHealth) {
    initialHealthCount_ = initialHealth;
  }

  void SetHealthCount(const uint32_t health) { healthCount_ = health; }

  void SetInvincibleTimer(const float time) { invincibleTimer_ = time; }
};

} // namespace Atrum