#include "System/Editor/Entity/EntityEditorCreateNewSceneFile.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorTagCatalog.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"
#include "System/Editor/Scene/EditorSceneAsset.h"

namespace Atrum::EntityEditor::EntityEditorCreateNewSceneFile {

void Create(EntityEditor &editor) {
  Atrum::Editor::Scene::EnsureScenesDirectoryExists();
  const std::string path = Atrum::Editor::Scene::MakeUniqueSceneFilePath();

  EditorSceneDocument &scene = editor.SceneDocument();
  scene = EditorSceneDocument{};
  scene.sceneName = Atrum::Editor::Scene::SceneDisplayNameFromPath(path);
  MergeTagCatalogFromRegistry(scene);
  SceneEntityInstance instance{};
  instance.instanceName = "Entity_0";
  instance.definition = MakeDefaultEntityDefinition("Entity_0");
  scene.instances = {std::move(instance)};

  UiState &ui = editor.Ui();
  ui.jsonFilePath = path;
  ui.selectedScenePath = path;
  ui.selectedInstanceIndex = 0;
  ui.selectedInstanceIndices = {0};
  ui.selectedComponentKind = ComponentSlotKind::Draw;
  ui.selectedComponentIndex = 0;
  ui.requestSaveToDisk = false;

  EntityEditorAuthoringDocumentEditNotify::NotifyPreviewOnly(editor);
}

} // namespace Atrum::EntityEditor::EntityEditorCreateNewSceneFile