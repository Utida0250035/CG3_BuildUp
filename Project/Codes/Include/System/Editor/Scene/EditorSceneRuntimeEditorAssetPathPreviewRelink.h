#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

namespace EntityEditor {

struct EditorSceneDocument;
struct EditorSceneRuntimeBuildWarnings;

/// 構築後・Initialize 後に CmpEditorAssetPath → Draw プレビューを再同期する。
void RelinkEditorAssetPathPreviewsInScene(
    const std::vector<std::unique_ptr<Entity>> &entities,
    const EditorSceneDocument &document,
    EditorSceneRuntimeBuildWarnings *warnings);

} // namespace EntityEditor
} // namespace Atrum