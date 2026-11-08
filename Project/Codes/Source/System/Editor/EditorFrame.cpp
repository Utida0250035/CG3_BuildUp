#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorFrame.h"

#include "Camera/Camera.h"
#include "ForDebug/ImGui.h"
#include <imgui/imgui_internal.h>
#include "Math/Matrix4x4.h"

namespace Atrum::Editor {

EditorFrame &EditorFrame::Instance() {
  static EditorFrame frame;
  return frame;
}

void EditorFrame::ResetPerFrame() {
  editView_ = {};
  gameView_ = {};
  runtimeView_ = {};
  editDisplay_ = {};
  gameDisplay_ = {};
  runtimeDisplay_ = {};
  editViewHovered_ = false;
}

void EditorFrame::SetSceneViewRect(const SceneViewTab tab,
                                   const SubViewportRect &rect,
                                   const bool hovered) {
  switch (tab) {
  case SceneViewTab::Edit:
    editView_ = rect;
    editViewHovered_ = hovered;
    break;
  case SceneViewTab::Game:
    gameView_ = rect;
    break;
  case SceneViewTab::Runtime:
    runtimeView_ = rect;
    break;
  }
}

const SubViewportRect &EditorFrame::SceneViewRect(const SceneViewTab tab) const {
  switch (tab) {
  case SceneViewTab::Edit:
    return editView_;
  case SceneViewTab::Game:
    return gameView_;
  case SceneViewTab::Runtime:
    return runtimeView_;
  }
  return editView_;
}

void EditorFrame::SetSceneViewTab(const SceneViewTab tab) {
  sceneViewTab_ = tab;
}

void EditorFrame::SetSceneViewDisplaySize(const SceneViewTab tab,
                                          const SceneViewDisplaySize &size) {
  switch (tab) {
  case SceneViewTab::Edit:
    editDisplay_ = size;
    break;
  case SceneViewTab::Game:
    gameDisplay_ = size;
    break;
  case SceneViewTab::Runtime:
    runtimeDisplay_ = size;
    break;
  }
}

const SceneViewDisplaySize &EditorFrame::SceneViewDisplaySizeOf(
    const SceneViewTab tab) const {
  switch (tab) {
  case SceneViewTab::Edit:
    return editDisplay_;
  case SceneViewTab::Game:
    return gameDisplay_;
  case SceneViewTab::Runtime:
    return runtimeDisplay_;
  }
  return editDisplay_;
}

bool MeasureSceneViewContentRegion(SubViewportRect &outRect, bool *outHovered,
                                   const char *hitId) {
  outRect = {};

  const ImVec2 size = ImGui::GetContentRegionAvail();
  if (size.x <= 2.0f || size.y <= 2.0f) {
    if (outHovered != nullptr) {
      *outHovered = false;
    }
    return false;
  }

  const ImVec2 fbScale = ImGui::GetIO().DisplayFramebufferScale;
  outRect.x = 0;
  outRect.y = 0;
  outRect.width = static_cast<int>(size.x * fbScale.x);
  outRect.height = static_cast<int>(size.y * fbScale.y);
  outRect.active = outRect.width > 2 && outRect.height > 2;

  if (!outRect.active) {
    if (outHovered != nullptr) {
      *outHovered = false;
    }
    return false;
  }

  ImGui::InvisibleButton(hitId, size);
  if (outHovered != nullptr) {
    *outHovered = ImGui::IsItemHovered();
  }
  return true;
}

void FitCameraPerspectiveToViewport(Atrum::Camera *camera, const int width,
                                    const int height) {
  if (camera == nullptr || width <= 0 || height <= 0) {
    return;
  }
  const float aspect =
      static_cast<float>(width) / static_cast<float>(height);
  camera->SetPerspectiveFovMatrix(
      Math::Matrix4x4::PerspectiveFov(0.5f, aspect, 0.125f, 128.0f));
  camera->UpdateMatrix();
}

} // namespace Atrum::Editor

#endif
