#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::Editor::Prefab::PrefabAssetBatchSpawnUi {

void DrawBatchSpawnControls(EntityEditor::EntityEditor &editor);

} // namespace Atrum::Editor::Prefab::PrefabAssetBatchSpawnUi

#endif