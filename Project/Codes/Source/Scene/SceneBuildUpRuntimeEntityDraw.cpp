#include "Scene/SceneBuildUpRuntimeEntityDraw.h"

#include "System/Editor/Scene/EditorSceneRuntime.h"
#include "System/Entity/Entity.h"

namespace Atrum::SceneBuildUpRuntimeEntityDraw {

void Draw(const std::vector<std::unique_ptr<Entity>> &entities) {
  EntityEditor::DrawSceneDocumentEntities(entities);
}

} // namespace Atrum::SceneBuildUpRuntimeEntityDraw