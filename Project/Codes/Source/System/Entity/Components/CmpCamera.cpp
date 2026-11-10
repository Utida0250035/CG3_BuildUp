#include "System/Entity/Components/CmpCamera.h"

#include "Engine/Platform/Window/Window.h"

#if defined(DEVELOPMENT) && !defined(NDEBUG)
#include "ForDebug/DebugCamera.h"
#endif

namespace Atrum {

void CmpCamera::Initialize() {
#if defined(DEVELOPMENT) && !defined(NDEBUG)
  camera_ = std::make_unique<Debug::DebugCamera>();
#else
  camera_ = std::make_unique<Camera>();
#endif
  if (camera_) {
    camera_->Initialize();
    camera_->CreateOrthographicMatrix(Window::kDefaultClientWidth,
                                      Window::kDefaultClientHeight);
  }
}

void CmpCamera::PrepareBeforeDraw(const bool stageViewLocked,
                                  const bool allowDebugControl) {
  if (!camera_) {
    return;
  }
  if (stageViewLocked || !allowDebugControl) {
    camera_->UpdateMatrix();
    return;
  }
  camera_->Update();
}

} // namespace Atrum
