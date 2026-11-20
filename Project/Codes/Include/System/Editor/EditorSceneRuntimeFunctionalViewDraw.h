#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include <memory>
#include <vector>

namespace Atrum {

class Entity;
class Scene;

namespace EntityEditor {
struct EditorSceneDocument;
}

namespace Editor {

/// 実行ビューで BuildUp 相当のランタイム描画（ステージカメラ・JSON ライト・シルエット）。
void DrawEditorSceneRuntimeFunctionalView(
    Scene &scene, const EntityEditor::EditorSceneDocument &document,
    const std::vector<std::unique_ptr<Entity>> &entities, int width,
    int height);

} // namespace Editor
} // namespace Atrum

#endif