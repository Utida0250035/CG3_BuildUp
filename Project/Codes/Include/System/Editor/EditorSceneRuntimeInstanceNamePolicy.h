#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

namespace Atrum {

class Entity;

} // namespace Atrum

namespace Atrum::EntityEditor {

/// JSON の instanceName をランタイム Entity の識別名（ハッシュ）へ反映する。
void ApplySceneInstanceName(Entity &entity, const SceneEntityInstance &instance);

} // namespace Atrum::EntityEditor