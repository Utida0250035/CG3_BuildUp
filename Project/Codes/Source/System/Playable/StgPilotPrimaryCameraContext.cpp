#include "System/Playable/StgPilotPrimaryCameraContext.h"

#include "Camera/Camera.h"

namespace Atrum::Player {

namespace {

const Camera *gCamera = nullptr;
int32_t gViewportWidth = 0;
int32_t gViewportHeight = 0;

} // namespace

void StgPilotPrimaryCameraContext::Sync(const Camera *camera,
                                        const int32_t viewportWidth,
                                        const int32_t viewportHeight) {
  gCamera = camera;
  gViewportWidth = viewportWidth;
  gViewportHeight = viewportHeight;
}

const Camera *StgPilotPrimaryCameraContext::PrimaryCamera() { return gCamera; }

bool StgPilotPrimaryCameraContext::ViewportSize(int32_t &outWidth,
                                                 int32_t &outHeight) {
  if (gCamera == nullptr || gViewportWidth <= 0 || gViewportHeight <= 0) {
    return false;
  }
  outWidth = gViewportWidth;
  outHeight = gViewportHeight;
  return true;
}

} // namespace Atrum::Player