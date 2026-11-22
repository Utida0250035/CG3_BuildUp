#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Atrum::Editor {

/// このフレームで非アクティブになった最後の項目が、編集済みだったか。
/// 公開 API に IsAnyItemDeactivatedAfterEdit が無いため、非アクティブ化記録を見る。
inline bool WasLastDeactivatedItemEditedThisFrame() {
  const ImGuiContext &context = *ImGui::GetCurrentContext();
  return context.DeactivatedItemData.ID != 0 &&
         context.DeactivatedItemData.HasBeenEditedBefore &&
         context.DeactivatedItemData.ElapseFrame >= context.FrameCount;
}

} // namespace Atrum::Editor

#endif
