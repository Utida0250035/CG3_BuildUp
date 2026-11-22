#pragma once

namespace Atrum::EntityEditor {

struct ComponentSlot;
struct UiState;

/// インスペクタのコンポーネント params をクリップボードへ保存。
void CopyComponentSettingsToClipboard(const ComponentSlot &slot, UiState &ui);

/// 同 typeName のスロットへ params（と executionOrder）を貼り付け。成功時 true。
bool PasteComponentSettingsFromClipboard(ComponentSlot &slot,
                                         const UiState &ui);

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
class EntityEditor;
/// コピー／ペーストボタン。いずれか成功で true（プレビュー再構築トリガ用）。
bool DrawComponentSettingsCopyPasteButtons(EntityEditor &editor,
                                           ComponentSlot &selected);
#endif

} // namespace Atrum::EntityEditor