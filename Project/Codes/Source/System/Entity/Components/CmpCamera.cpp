#include "System/Entity/Components/CmpCamera.h"

#include "Engine/Platform/Window/Window.h"
#include "Math/Matrix4x4.h"

#if defined(DEVELOPMENT) && !defined(NDEBUG)
#include "ForDebug/DebugCamera.h"
#endif

namespace Atrum {

void CmpCamera::SetPerspectiveParams(const float fovY, const float nearClip,
                                     const float farClip) {
  perspectiveFovY_ = fovY;
  nearClip_ = nearClip;
  farClip_ = farClip;
  if (camera_) {
    ApplyDefaultProjectionMatrices();
  }
}

void CmpCamera::ApplyDefaultProjectionMatrices() {
  if (!camera_) {
    return;
  }
  const float aspect =
      static_cast<float>(Window::kDefaultClientWidth) /
      static_cast<float>(Window::kDefaultClientHeight);
  camera_->SetPerspectiveFovMatrix(Math::Matrix4x4::PerspectiveFov(
      perspectiveFovY_, aspect, nearClip_, farClip_));
  camera_->CreateOrthographicMatrix(Window::kDefaultClientWidth,
                                   Window::kDefaultClientHeight);
}

void CmpCamera::Initialize() {
#if defined(DEVELOPMENT) && !defined(NDEBUG)
  camera_ = std::make_unique<Debug::DebugCamera>();
#else
  camera_ = std::make_unique<Camera>();
#endif
  if (camera_) {
    camera_->Initialize();
    ApplyDefaultProjectionMatrices();
  }
}

void CmpCamera::PrepareBeforeDraw(const bool enableDebugCameraInput) {
  if (!camera_) {
    return;
  }
  if (!enableDebugCameraInput) {
    camera_->UpdateMatrix();
    return;
  }
  camera_->Update();
}

} // namespace Atrum
