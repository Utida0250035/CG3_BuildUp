#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorFrame.h"

#include "System/Editor/EditorFrameFitCameraPerspectiveParams.h"

#include "System/Editor/EditorFrameLightingDiagnostics.h"
#include "System/Editor/EditorFrameStreamingDiagnostics.h"
#include "System/Editor/EditorFrameFrameDeltaDiagnostics.h"

#include "Camera/Camera.h"
#include "ForDebug/ImGui.h"
#include <imgui/imgui_internal.h>
#include "Math/Matrix4x4.h"

namespace Atrum::Editor {

EditorFrame &EditorFrame::Instance() {
  static EditorFrame frame;
  return frame;
}

void EditorFrame::DrawRenderDiagnosticsMenuItems() {
  EditorFrameLightingDiagnostics::DrawMenuItems();
  EditorFrameStreamingDiagnostics::DrawMenuItems();
}

void EditorFrame::DrawRenderDiagnosticsMainMenuBar() {
  if (!ImGui::BeginMainMenuBar()) {
    return;
  }
  if (ImGui::BeginMenu("描画診断")) {
    EditorFrameLightingDiagnostics::DrawMenuItems();
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("デバッグ")) {
    EditorFrameStreamingDiagnostics::DrawMenuItems();
    EditorFrameFrameDeltaDiagnostics::DrawMenuItems();
    ImGui::EndMenu();
  }
  ImGui::EndMainMenuBar();
}

void EditorFrame::ResetPerFrame() {
  editView_ = {};
  gameView_ = {};
  runtimeView_ = {};
  editDisplay_ = {};
  gameDisplay_ = {};
  runtimeDisplay_ = {};
  editViewHovered_ = false;
  gameViewHovered_ = false;
  runtimeViewHovered_ = false;
  for (bool &valid : sceneViewImageBoundsValid_) {
    valid = false;
  }
}

namespace {

size_t SceneViewTabIndex(const SceneViewTab tab) {
  return static_cast<size_t>(tab);
}

} // namespace

bool EditorFrame::SceneViewHovered(const SceneViewTab tab) const {
  switch (tab) {
  case SceneViewTab::Edit:
    return editViewHovered_;
  case SceneViewTab::Game:
    return gameViewHovered_;
  case SceneViewTab::Runtime:
    return runtimeViewHovered_;
  }
  return false;
}

void EditorFrame::SetSceneViewImageScreenBounds(const SceneViewTab tab,
                                                const float minX,
                                                const float minY,
                                                const float maxX,
                                                const float maxY) {
  const size_t index = SceneViewTabIndex(tab);
  sceneViewImageMinX_[index] = minX;
  sceneViewImageMinY_[index] = minY;
  sceneViewImageMaxX_[index] = maxX;
  sceneViewImageMaxY_[index] = maxY;
  sceneViewImageBoundsValid_[index] = maxX > minX && maxY > minY;
}

bool EditorFrame::TryMapMouseToSceneViewLocal(const SceneViewTab tab,
                                              float &outLocalX,
                                              float &outLocalY) const {
  const size_t index = SceneViewTabIndex(tab);
  if (!SceneViewHovered(tab) || !sceneViewImageBoundsValid_[index]) {
    return false;
  }
  const ImVec2 mouse = ImGui::GetIO().MousePos;
  outLocalX = mouse.x - sceneViewImageMinX_[index];
  outLocalY = mouse.y - sceneViewImageMinY_[index];
  const float w = sceneViewImageMaxX_[index] - sceneViewImageMinX_[index];
  const float h = sceneViewImageMaxY_[index] - sceneViewImageMinY_[index];
  if (outLocalX < 0.0f || outLocalY < 0.0f || outLocalX > w || outLocalY > h) {
    return false;
  }
  return true;
}

void EditorFrame::SetEditViewImageScreenBounds(const float minX, const float minY,
                                               const float maxX,
                                               const float maxY) {
  SetSceneViewImageScreenBounds(SceneViewTab::Edit, minX, minY, maxX, maxY);
}

bool EditorFrame::TryMapMouseToEditViewLocal(float &outLocalX,
                                             float &outLocalY) const {
  return TryMapMouseToSceneViewLocal(SceneViewTab::Edit, outLocalX, outLocalY);
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
    gameViewHovered_ = hovered;
    break;
  case SceneViewTab::Runtime:
    runtimeView_ = rect;
    runtimeViewHovered_ = hovered;
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
  const float fovY =
      EditorFrameFitCameraPerspectiveParams::ResolveViewportPerspectiveFovY();
  camera->SetPerspectiveFovMatrix(Math::Matrix4x4::PerspectiveFov(
      fovY, aspect, EditorFrameFitCameraPerspectiveParams::kViewportNearClip,
      EditorFrameFitCameraPerspectiveParams::kViewportFarClip));
  camera->UpdateMatrix();
}

} // namespace Atrum::Editor

#endif
