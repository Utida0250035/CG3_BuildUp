#pragma once

#include <cstdint>

namespace Atrum {

class Camera;

namespace Player {

/// BuildUp／ゲームシーンが毎フレーム同期する。マウスエイムの射影に使う。
class StgPilotPrimaryCameraContext final {
public:
  static void Sync(const Camera *camera, int32_t viewportWidth,
                   int32_t viewportHeight);

  static const Camera *PrimaryCamera();
  static bool ViewportSize(int32_t &outWidth, int32_t &outHeight);

  StgPilotPrimaryCameraContext() = delete;
};

} // namespace Player
} // namespace Atrum