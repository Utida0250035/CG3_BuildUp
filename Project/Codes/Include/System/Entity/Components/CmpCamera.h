#pragma once

#include "Camera/Camera.h"
#include "System/Entity/ComponentUpd.h"
#include <memory>

namespace Atrum {

/// ランタイム用カメラ（Camera を所有）。primary はタグ camera.main。
class CmpCamera final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::PRE_RENDER;
  }

  void Initialize() override;

  void SetPerspectiveParams(float fovY, float nearClip, float farClip);

  /// GameOrder 描画直前に Scene から呼ぶ（通常の Entity::Update では行わない）
  void PrepareBeforeDraw(bool enableDebugCameraInput);

  Camera *GetCamera() { return camera_.get(); }
  const Camera *GetCamera() const { return camera_.get(); }

private:
  void ApplyDefaultProjectionMatrices();

  std::unique_ptr<Camera> camera_;
  float perspectiveFovY_ = 0.5f;
  float nearClip_ = 0.125f;
  float farClip_ = 128.0f;
};

} // namespace Atrum
