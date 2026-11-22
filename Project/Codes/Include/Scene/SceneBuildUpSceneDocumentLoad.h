#pragma once

#include "Camera/StageViewFramingBounds.h"
#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Entity;

/// 機能検証 BuildUp 用のシーン JSON 読込・Entity 化（EditorSceneRuntime への境界）
namespace SceneBuildUpSceneDocumentLoad {

bool TryLoad(const std::string &jsonFilePath,
             EntityEditor::EditorSceneDocument &outDocument);

std::vector<std::unique_ptr<Entity>>
BuildEntities(const EntityEditor::EditorSceneDocument &document);

StageViewFramingBounds ResolveStageViewFraming(
    const EntityEditor::EditorSceneDocument &document);

} // namespace SceneBuildUpSceneDocumentLoad

} // namespace Atrum