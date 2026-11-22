#include "System/Editor/Entity/EntityEditorPrefabBatchSpawnFromFile.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"
#include "System/Editor/Entity/EntityEditorPrefabSpawnFirstInstanceRoot.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor::EntityEditorPrefabBatchSpawnFromFile {

namespace {

std::string MakeUniqueInstanceName(const EditorSceneDocument &scene,
                                   std::string baseName) {
  auto exists = [&scene](const std::string &name) {
    for (const SceneEntityInstance &instance : scene.instances) {
      if (instance.instanceName == name) {
        return true;
      }
    }
    return false;
  };
  if (baseName.empty()) {
    baseName = "PrefabInstance";
  }
  if (!exists(baseName)) {
    return baseName;
  }
  for (int suffix = 2; suffix < 100000; ++suffix) {
    const std::string candidate = baseName + "_" + std::to_string(suffix);
    if (!exists(candidate)) {
      return candidate;
    }
  }
  return baseName + "_overflow";
}

} // namespace

int AppendFromPrefabFileBatch(EntityEditor &editor,
                              const std::string &prefabPath, const int count,
                              const float spacing) {
  if (count <= 0 || prefabPath.empty()) {
    return 0;
  }

  namespace fs = std::filesystem;
  if (!fs::exists(prefabPath)) {
    return 0;
  }

  std::ifstream file(prefabPath);
  if (!file.is_open()) {
    return 0;
  }

  nlohmann::json json{};
  try {
    file >> json;
  } catch (...) {
    return 0;
  }

  EditorSceneDocument &scene = editor.SceneDocument();
  std::vector<int> newSelection;
  newSelection.reserve(static_cast<size_t>(count));

  int spawned = 0;
  for (int index = 0; index < count; ++index) {
    std::optional<SceneEntityInstance> built =
        EntityEditorPrefabSpawnFirstInstanceRoot::
            TryBuildRootInstanceFromPrefabJson(json);
    if (!built.has_value()) {
      break;
    }

    SceneEntityInstance instance = std::move(*built);
    const std::string stem =
        instance.instanceName.empty()
            ? fs::path(prefabPath).stem().string()
            : instance.instanceName;
    const std::string uniqueName = MakeUniqueInstanceName(scene, stem);
    instance.instanceName = uniqueName;
    instance.definition.entityName = uniqueName;
    instance.definition.initialTransform.translate.x += spacing * static_cast<float>(index);

    scene.instances.push_back(std::move(instance));
    newSelection.push_back(static_cast<int>(scene.instances.size()) - 1);
    ++spawned;
  }

  if (spawned > 0) {
    UiState &ui = editor.Ui();
    ui.selectedInstanceIndices = newSelection;
    ui.selectedInstanceIndex = newSelection.front();
    editor.ClampSelectedInstanceIndex();
    EntityEditorAuthoringDocumentEditNotify::Notify(editor);
  }

  return spawned;
}

} // namespace Atrum::EntityEditor::EntityEditorPrefabBatchSpawnFromFile