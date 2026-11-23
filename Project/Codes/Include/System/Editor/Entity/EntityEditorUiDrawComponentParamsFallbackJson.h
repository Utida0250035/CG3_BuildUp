#pragma once

namespace Atrum::EntityEditor {
struct ComponentSlot;
}

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsFallbackJson {

/// 専用インスペクタ未対応のコンポーネント向け汎用 JSON フォールバックエディタ
void DrawFallbackComponentParams(ComponentSlot &slot, bool &outChanged);

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsFallbackJson
