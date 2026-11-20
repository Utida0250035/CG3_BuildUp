#pragma once

#include <functional>

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::EntityEditor {

class EntityEditor;

/// エディタ内実行は Runtime タブ切替のみ（BuildUp 非連動）。
namespace SceneEntityEditorInEditorPlayTick {

bool Process(EntityEditor &editor,
             const std::function<void()> &rebuildPreviewScene);

} // namespace SceneEntityEditorInEditorPlayTick

} // namespace Atrum::EntityEditor

#endif