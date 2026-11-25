#include "System/AtomF/AtomFPlayerAimMousePick.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Input/PlayInput.h"
#include "Math/Vector2.h"
#include "System/Entity/Entity.h"

#include <cmath>

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "System/Editor/EditorFrame.h"
#endif

namespace Atrum::AtomF {

namespace {

constexpr float kMinScreenAimDistSq = 1e-6f;

float DirectionToAngleRadians(const float dirX, const float dirZ) {
  return std::atan2(dirX, dirZ);
}

/// 画面中心→カーソルの正規化 2D 向き。ベクトルの X を照準 X、Y を照準 Z（Y は使わない）に対応させる。
bool TryAimAngleFromScreenCenterToCursor(const float localX,
                                         const float localY,
                                         const float viewWidth,
                                         const float viewHeight,
                                         float &outAngleRadians) {
  if (viewWidth < 1.0f || viewHeight < 1.0f) {
    return false;
  }

  const float dirScreenX = localX - viewWidth * 0.5f;
  const float dirScreenY = localY - viewHeight * 0.5f;
  const float lenSq = dirScreenX * dirScreenX + dirScreenY * dirScreenY;
  if (lenSq < kMinScreenAimDistSq) {
    return false;
  }

  const float invLen = 1.0f / std::sqrt(lenSq);
  const float aimDirX = dirScreenX * invLen;
  const float aimDirZ = dirScreenY * invLen;
  outAngleRadians = DirectionToAngleRadians(aimDirX, -aimDirZ);
  return true;
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

bool TryAimFromEditorSceneViewImage(float &outAngleRadians) {
  Editor::EditorFrame &frame = Editor::EditorFrame::Instance();
  using Editor::SceneViewTab;

  const SceneViewTab tabs[] = {SceneViewTab::Runtime, SceneViewTab::Game,
                               SceneViewTab::Edit};

  for (const SceneViewTab tab : tabs) {
    float localX = 0.0f;
    float localY = 0.0f;
    if (!frame.TryMapMouseToSceneViewLocal(tab, localX, localY)) {
      continue;
    }

    const Editor::SceneViewDisplaySize display =
        frame.SceneViewDisplaySizeOf(tab);
    if (display.width < 2.0f || display.height < 2.0f) {
      continue;
    }

    if (TryAimAngleFromScreenCenterToCursor(localX, localY, display.width,
                                          display.height, outAngleRadians)) {
      return true;
    }
  }

  return false;
}

#endif

bool TryAimFromClientViewport(const Math::Vector2 &cursor,
                              float &outAngleRadians) {
  AtrumEngine *engine = AtrumEngine::Instance();
  if (engine == nullptr) {
    return false;
  }

  int32_t viewWidth = 0;
  int32_t viewHeight = 0;
  if (!engine->TryGetClientViewportSize(viewWidth, viewHeight)) {
    return false;
  }
  if (viewWidth < 2 || viewHeight < 2) {
    return false;
  }

  return TryAimAngleFromScreenCenterToCursor(
      cursor.x, cursor.y, static_cast<float>(viewWidth),
      static_cast<float>(viewHeight), outAngleRadians);
}

} // namespace

bool TryResolveAimAngleFromMouse(const Entity &owner, float &outAngleRadians) {
  (void)owner;
  Input::PlayInput *input = ::Atrum::EnginePlayInput();
  if (input == nullptr) {
    return false;
  }

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (TryAimFromEditorSceneViewImage(outAngleRadians)) {
    return true;
  }
#endif

  const Math::Vector2 cursor = input->GetCursorPos();
  return TryAimFromClientViewport(cursor, outAngleRadians);
}

} // namespace Atrum::AtomF