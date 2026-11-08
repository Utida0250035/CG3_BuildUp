#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include <memory>
#include <string>
#include <vector>

namespace Atrum {

class Entity;

} // namespace Atrum

namespace Atrum::EntityEditor {

/// シーン JSON の読み込みと Entity 実体化（エディタ・実行シーン共通）
bool TryLoadSceneDocument(const std::string &jsonFilePath,
                          EditorSceneDocument &outDocument);

std::vector<std::unique_ptr<Entity>>
BuildEntitiesFromSceneDocument(const EditorSceneDocument &document);

} // namespace Atrum::EntityEditor
