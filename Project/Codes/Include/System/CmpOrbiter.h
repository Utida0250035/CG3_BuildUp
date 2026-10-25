#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

class FrameDeltaTime;

class CmpOrbiter : public ComponentUpd {
private:
  Math::Vector3 rotateAxis_ = Math::Vector3::UpLh();
  float rotateSpeed_ = 0.0f;
  float baselineRotateSpeed_ = 0.0f;
  FrameDeltaTime *frameDeltaTime_ = nullptr;

  Math::Vector3 defaultAxis_{};
  float defaultSpeed_ = 0.0f;
  Math::Vector3 defaultTranslate_{};
  bool hasDefaults_ = false;

public:
  constexpr UpdCmpGroup UpdGroup() const override { return UpdCmpGroup::MOVEMENT; }

  void ResolveDependence() override;
  void Update() override;

  void SetRotateAxis(const Math::Vector3 &axis) { rotateAxis_ = axis; }

  void SetRotateSpeed(const float rotateSpeed);

  /// テンポ変化後も RestoreDefaults が基準速度に戻せるよう公転速度を更新
  void ApplyRotateSpeedScale(const float scale);

  void CaptureDefaults();
  void RestoreDefaults();
};

} // namespace Atrum
