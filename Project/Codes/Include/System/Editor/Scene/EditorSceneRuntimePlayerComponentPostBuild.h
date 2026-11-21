#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include <memory>
#include <vector>

namespace Atrum {

class Entity;

} // namespace Atrum

namespace Atrum::EntityEditor {

/// プレイヤー向けコンポーネントのインスタンス参照・武器生成などを組み立て後に解決
void ApplyPlayerComponentPostBuild(
    const EditorSceneDocument &document,
    std::vector<std::unique_ptr<Entity>> &entities);

} // namespace Atrum::EntityEditor