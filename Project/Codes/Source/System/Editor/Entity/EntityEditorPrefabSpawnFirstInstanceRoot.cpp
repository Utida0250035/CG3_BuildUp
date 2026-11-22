#include "System/Editor/Entity/EntityEditorPrefabSpawnFirstInstanceRoot.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"
#include "System/Editor/Entity/EntityEditorPrefabBatchSpawnFromFile.h"

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

  if (json.contains("definition")) {
    SceneEntityInstance instance = json.get<SceneEntityInstance>();
    return DetachAsRootInstance(std::move(instance));
  }

  SceneEntityInstance instance{};
  instance.definition = json.get<EntityDefinition>();
  instance.instanceName = instance.definition.entityName;
  return DetachAsRootInstance(std::move(instance));
}

bool AppendFromPrefabFile(EntityEditor &editor, const std::string &prefabPath) {
  return EntityEditorPrefabBatchSpawnFromFile::AppendFromPrefabFileBatch(
             editor, prefabPath, 1, 0.0f) > 0;
}

} // namespace Atrum::EntityEditor::EntityEditorPrefabSpawnFirstInstanceRoot