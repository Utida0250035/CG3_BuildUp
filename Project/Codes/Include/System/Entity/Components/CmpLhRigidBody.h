#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class FrameDeltaTime;

class CmpLhRigidBody : public ComponentUpd {
private:
  FrameDeltaTime *frameDeltaTime_ = nullptr;

  // 加速度
  Math::Vector3 acceleration_{};
  // 速度
  Math::Vector3 velocity_{};
  // 重力加速度
  Math::Vector3 gravity_{};
  // 重力を使用するか
  bool isUseGravity_ = true;

  // 画面端で位置を止めるか
  bool isUseSpaceEnd_ = true;

public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PHYSICS;
  }

  void ResolveDependence() override;
  // 初期化
  void Initialize() override;
  // 実行
  void Update() override;

  // 速度の参照
  Math::Vector3 &RefVelocity() { return velocity_; }
  // 速度の取得
  Math::Vector3 GetVelocity() const { return velocity_; }

  // 重力加速度の設定
  void SetGravity(const Math::Vector3 &gravity) { gravity_ = gravity; }
  void SetIsUseGravity(const bool isUseGravity) {
    isUseGravity_ = isUseGravity;
  }

  void SetIsUseSpaceEnd(const bool isUseSpaceEnd) {
    isUseSpaceEnd_ = isUseSpaceEnd;
  }
};

} // namespace Atrum