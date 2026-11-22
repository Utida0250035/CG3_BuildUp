#pragma once

#include <string>

namespace Atrum {
class Entity;
}

namespace Atrum::EntityEditor {
struct EditorSceneRuntimeBuildWarnings;
}

namespace Atrum::EntityEditor::EditorSceneRuntimeEditorAssetPathPreviewLink {

/// CmpEditorAssetPath のパスを、宣言済み Draw スロット（CmpMesh / テクスチャビルボード）へだけ反映する。
void Apply(Entity &entity, const std::string &entityLabel,
           EditorSceneRuntimeBuildWarnings *warnings);

} // namespace Atrum::EntityEditor::EditorSceneRuntimeEditorAssetPathPreviewLink