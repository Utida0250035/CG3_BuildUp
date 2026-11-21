#pragma once

namespace Atrum::EntityEditor {

class EntityEditor;

/// 未保存のまま別シーンを開く確認モーダル（DEVELOPMENT + USE_IMGUI）
void DrawEntityEditorSceneSwitchUnsavedPrompt(EntityEditor &editor);

} // namespace Atrum::EntityEditor