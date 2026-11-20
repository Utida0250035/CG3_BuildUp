#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

namespace EntityEditor {

class EntityEditor;

/// ExitScene 時のプレビュー解放と Ui 実行フラグの巻き戻し。
namespace SceneEntityEditorExitSceneTeardown {

void ReleasePreviewEntities(std::vector<std::unique_ptr<Entity>> &previewEntities);

void ResetEditorTransientPlayUi(EntityEditor &editor);

} // namespace SceneEntityEditorExitSceneTeardown

} // namespace Atrum::EntityEditor

} // namespace Atrum