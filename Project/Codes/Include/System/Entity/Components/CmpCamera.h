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

  /// GameOrder 描画直前に Scene から呼ぶ（通常の Entity::Update では行わない）
  void PrepareBeforeDraw(bool stageViewLocked, bool allowDebugControl);

  Camera *GetCamera() { return camera_.get(); }
  const Camera *GetCamera() const { return camera_.get(); }

private:
  std::unique_ptr<Camera> camera_;
};

} // namespace Atrum
