#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <memory>
#include <vector>

namespace Atrum {

class Entity;

/// 読込失敗時にダミー Entity を生成しない
namespace SceneBuildUpLoadFailureEntities {

std::vector<std::unique_ptr<Entity>>
BuildEntitiesAfterDocumentLoad(bool loadSucceeded,
                                const EntityEditor::EditorSceneDocument
                                    &document);

} // namespace SceneBuildUpLoadFailureEntities

} // namespace Atrum