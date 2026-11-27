#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Entity;

} // namespace Atrum

namespace Atrum::EntityEditor {

struct EditorSceneRuntimeBuildWarnings;

/// シーン JSON の読み込みと Entity 実体化（エディタ・実行シーン共通）
bool TryLoadSceneDocument(const std::string &jsonFilePath,
                          EditorSceneDocument &outDocument);

std::vector<std::unique_ptr<Entity>>
BuildEntitiesFromSceneDocument(
    const EditorSceneDocument &document,
    EditorSceneRuntimeBuildWarnings *warnings = nullptr);

/// 1 インスタンス分を Entity 化し、依存解決・Initialize まで行う（ゲーム中の動的生成用）。
/// シーン文書の読込やメインカメラ補完は行わない。親子付けは呼び出し側で行う。
std::unique_ptr<Entity>
BuildEntityForRuntimeSpawn(const SceneEntityInstance &instance,
                           EditorSceneRuntimeBuildWarnings *warnings = nullptr);

/// ルート Entity の Draw と、親が非 Active な子のフォールバック描画。
void DrawSceneDocumentEntities(
    const std::vector<std::unique_ptr<Entity>> &entities);

/// シーン JSON の明示 bounds・境界マーカー・既定 StageBounds からフレーミングを解決
StageViewFramingBounds
ResolveStageViewFramingBounds(const EditorSceneDocument &document);

} // namespace Atrum::EntityEditor
