#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Atrum {
class Entity;
}

namespace Atrum::EntityEditor {
struct EditorSceneDocument;
class EntityEditorUi;
}

namespace Atrum::EntityEditor::EntityEditorBuildPreviewSceneBuildUpParity {

/// BuildUp と同じ JSON→Entity 経路でプレビュー実体を構築し PlayRequest を同期する。
std::vector<std::unique_ptr<Entity>>
Build(const EditorSceneDocument &document, EntityEditorUi &ui,
      std::string *importStatusMessageOut);

} // namespace Atrum::EntityEditor::EntityEditorBuildPreviewSceneBuildUpParity