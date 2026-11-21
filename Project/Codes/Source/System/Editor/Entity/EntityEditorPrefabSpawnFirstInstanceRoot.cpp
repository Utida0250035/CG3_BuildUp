#include "System/Editor/Entity/EntityEditorPrefabSpawnFirstInstanceRoot.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor::EntityEditorPrefabSpawnFirstInstanceRoot {

namespace {

bool InstanceNameExists(const EditorSceneDocument &scene,
                        const std::string &name) {
  for (const SceneEntityInstance &instance : scene.instances) {
    if (instance.instanceName == name) {
      return true;
    }
  }
  return false;
}

std::string MakeUniqueInstanceName(const EditorSceneDocument &scene,
                                   std::string baseName) {
  if (baseName.empty()) {
    baseName = "PrefabInstance";
  }
  if (!InstanceNameExists(scene, baseName)) {
    return baseName;
  }
  for (int suffix = 2; suffix < 100000; ++suffix) {
    const std::string candidate = baseName + "_" + std::to_string(suffix);
    if (!InstanceNameExists(scene, candidate)) {
      return candidate;
    }
  }
  return baseName + "_overflow";
}

SceneEntityInstance DetachAsRootInstance(SceneEntityInstance instance) {
  instance.parentIndex = -1;
  return instance;
}

} // namespace

std::optional<SceneEntityInstance>
TryBuildRootInstanceFromPrefabJson(const nlohmann::json &json) {
  if (json.contains("entities") && json.at("entities").is_array()) {
    EditorSceneDocument prefabScene = json.get<EditorSceneDocument>();
    if (prefabScene.instances.empty()) {
      return std::nullopt;
    }
    // プレハブ追加は instances[0] のみ（複数体シーン JSON は一括取り込まない）
    return DetachAsRootInstance(prefabScene.instances.front());
  }

  SceneEntityInstance instance{};
  instance.definition = json.get<EntityDefinition>();
  instance.instanceName = instance.definition.entityName;
  return DetachAsRootInstance(std::move(instance));
}

bool AppendFromPrefabFile(EntityEditor &editor, const std::string &prefabPath) {
  namespace fs = std::filesystem;
  if (prefabPath.empty() || !fs::exists(prefabPath)) {
    return false;
  }

  std::ifstream file(prefabPath);
  if (!file.is_open()) {
    return false;
  }

  nlohmann::json json{};
  try {
    file >> json;
  } catch (...) {
    return false;
  }

  std::optional<SceneEntityInstance> built =
      TryBuildRootInstanceFromPrefabJson(json);
  if (!built.has_value()) {
    return false;
  }

  SceneEntityInstance instance = std::move(*built);
  EditorSceneDocument &scene = editor.SceneDocument();
  const std::string uniqueName = MakeUniqueInstanceName(
      scene, instance.instanceName.empty()
                 ? std::filesystem::path(prefabPath).stem().string()
                 : instance.instanceName);
  instance.instanceName = uniqueName;
  instance.definition.entityName = uniqueName;

  scene.instances.push_back(std::move(instance));
  UiState &ui = editor.Ui();
  ui.selectedInstanceIndex = static_cast<int>(scene.instances.size()) - 1;
  ui.selectedInstanceIndices = {ui.selectedInstanceIndex};
  editor.ClampSelectedInstanceIndex();
  EntityEditorAuthoringDocumentEditNotify::Notify(editor);
  return true;
}

} // namespace Atrum::EntityEditor::EntityEditorPrefabSpawnFirstInstanceRoot