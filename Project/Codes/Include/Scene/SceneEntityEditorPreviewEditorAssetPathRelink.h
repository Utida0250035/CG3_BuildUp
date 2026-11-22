#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

namespace EntityEditor {
class EntityEditor;
}

/// エディタ内プレビュー実体へ AssetPath プレビューを再適用する（構築時1回制限の解消）。
void RelinkPreviewEditorAssetPathsAfterRebuild(
    EntityEditor::EntityEditor &editor,
    const std::vector<std::unique_ptr<Entity>> &previewEntities);

} // namespace Atrum