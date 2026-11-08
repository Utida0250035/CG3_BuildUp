#include "System/Editor/Entity/EntityEditor.h"

#include "System/Editor/EditorImportedAsset.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/Editor/Entity/EntityEditorTagCatalog.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"
#include "System/Entity/Entity.h"
#include "System/Json/JsonHandler.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor {

EntityDefinition &EntityEditor::SelectedDefinition() {
  ClampSelectedInstanceIndex();
  return scene_.instances[static_cast<size_t>(ui_.selectedInstanceIndex)]
      .definition;
}

const EntityDefinition &EntityEditor::SelectedDefinition() const {
  return scene_.instances[static_cast<size_t>(ui_.selectedInstanceIndex)]
      .definition;
}

SceneEntityInstance &EntityEditor::SelectedInstance() {
  ClampSelectedInstanceIndex();
  return scene_.instances[static_cast<size_t>(ui_.selectedInstanceIndex)];
}

const SceneEntityInstance &EntityEditor::SelectedInstance() const {
  return scene_.instances[static_cast<size_t>(ui_.selectedInstanceIndex)];
}

void EntityEditor::ClampSelectedInstanceIndex() {
  if (scene_.instances.empty()) {
    ui_.selectedInstanceIndex = 0;
    return;
  }
  ui_.selectedInstanceIndex = std::clamp(
      ui_.selectedInstanceIndex, 0,
      static_cast<int>(scene_.instances.size()) - 1);
}

void EntityEditor::AddEmptyInstance() {
  const std::string name =
      "Entity_" + std::to_string(scene_.instances.size());
  SceneEntityInstance instance{};
  instance.instanceName = name;
  instance.definition = MakeDefaultEntityDefinition(name);
  scene_.instances.push_back(std::move(instance));
  ui_.selectedInstanceIndex =
      static_cast<int>(scene_.instances.size()) - 1;
  ui_.requestApplyPreview = true;
}

void EntityEditor::RemoveSelectedInstance() {
  if (scene_.instances.size() <= 1) {
    return;
  }
  const int removeIndex = ui_.selectedInstanceIndex;
  scene_.instances.erase(scene_.instances.begin() + removeIndex);
  for (SceneEntityInstance &instance : scene_.instances) {
    if (instance.parentIndex == removeIndex) {
      instance.parentIndex = -1;
    } else if (instance.parentIndex > removeIndex) {
      --instance.parentIndex;
    }
  }
  ClampSelectedInstanceIndex();
  ui_.requestApplyPreview = true;
}

bool EntityEditor::SaveSelectedInstanceToPrefabPath(
    const std::string &prefabPath) {
  if (scene_.instances.empty() || ui_.selectedInstanceIndex < 0 ||
      prefabPath.empty()) {
    return false;
  }

  EntityDefinition definition = SelectedDefinition();
  RebuildEntityDefinitionTagHashes(definition);
  try {
    Json::JsonHandler::SaveToFile(prefabPath, definition);
    return true;
  } catch (...) {
    return false;
  }
}

bool EntityEditor::AddInstanceFromPrefabPath(const std::string &prefabPath) {
  namespace fs = std::filesystem;
  if (!fs::exists(prefabPath)) {
    return false;
  }
  std::ifstream file(prefabPath);
  if (!file.is_open()) {
    return false;
  }
  nlohmann::json json{};
  file >> json;
  SceneEntityInstance instance{};
  if (json.contains("entities") && json.at("entities").is_array()) {
    EditorSceneDocument prefabScene = json.get<EditorSceneDocument>();
    if (prefabScene.instances.empty()) {
      return false;
    }
    instance = prefabScene.instances.front();
    instance.parentIndex = -1;
  } else {
    instance.definition = json.get<EntityDefinition>();
    instance.instanceName = instance.definition.entityName;
    instance.parentIndex = -1;
  }
  if (instance.instanceName.empty()) {
    instance.instanceName =
        "Instance_" + std::to_string(scene_.instances.size());
  }
  scene_.instances.push_back(std::move(instance));
  ui_.selectedInstanceIndex =
      static_cast<int>(scene_.instances.size()) - 1;
  ui_.requestApplyPreview = true;
  return true;
}

void EntityEditor::LoadFromJsonPath(const std::string &jsonFilePath) {
  namespace fs = std::filesystem;
  if (!fs::exists(jsonFilePath)) {
    scene_ = EditorSceneDocument{};
    scene_.sceneName = "EditorScene";
    SceneEntityInstance instance{};
    instance.instanceName = "Entity_0";
    instance.definition = MakeDefaultEntityDefinition("Entity_0");
    scene_.instances = {std::move(instance)};
    ui_.selectedInstanceIndex = 0;
    ui_.selectedComponentKind = ComponentSlotKind::Draw;
    ui_.selectedComponentIndex = 0;
    return;
  }

  std::ifstream file(jsonFilePath);
  if (!file.is_open()) {
    return;
  }
  nlohmann::json json{};
  file >> json;

  if (json.contains("entities") && json.at("entities").is_array()) {
    scene_ = json.get<EditorSceneDocument>();
  } else {
    scene_ = EditorSceneDocument{};
    scene_.sceneName = "EditorScene";
    SceneEntityInstance instance{};
    instance.definition = json.get<EntityDefinition>();
    instance.instanceName = instance.definition.entityName;
    if (instance.instanceName.empty()) {
      instance.instanceName = "Entity_0";
    }
    scene_.instances = {std::move(instance)};
  }

  ui_.selectedInstanceIndex = 0;
  ui_.selectedComponentIndex = 0;
  if (!SelectedDefinition().components.empty()) {
    ui_.selectedComponentKind =
        SelectedDefinition().components.front().kind;
  } else {
    ui_.selectedComponentKind = ComponentSlotKind::Draw;
  }
  MergeTagCatalogFromRegistry(scene_);
  SyncAllEntityDefinitionTags(scene_);
  SyncTagCatalogFromInstances(scene_);
  ClampSelectedInstanceIndex();
}

void EntityEditor::SaveToJsonPath(const std::string &jsonFilePath) {
  for (SceneEntityInstance &instance : scene_.instances) {
    RebuildEntityDefinitionTagHashes(instance.definition);
  }
  Json::JsonHandler::SaveToFile(jsonFilePath, scene_);
}

bool EntityEditor::TickPendingRequests() {
  bool needApply = false;

  if (ui_.requestReloadFromDisk) {
    ui_.requestReloadFromDisk = false;
    LoadFromJsonPath(ui_.jsonFilePath);
    needApply = true;
  }
  if (ui_.requestSaveToDisk) {
    ui_.requestSaveToDisk = false;
    SaveToJsonPath(ui_.jsonFilePath);
  }
  if (ui_.requestApplyPreview) {
    ui_.requestApplyPreview = false;
    needApply = true;
  }
  if (ui_.requestResetPreview) {
    ui_.requestResetPreview = false;
    needApply = true;
  }

  return needApply;
}

std::vector<std::unique_ptr<Entity>> EntityEditor::BuildPreviewScene() const {
  return BuildEntitiesFromSceneDocument(scene_);
}

void EntityEditor::CreateNewSceneFile() {
  Atrum::Editor::Scene::EnsureScenesDirectoryExists();
  const std::string path = Atrum::Editor::Scene::MakeUniqueSceneFilePath();

  scene_ = EditorSceneDocument{};
  scene_.sceneName = Atrum::Editor::Scene::SceneDisplayNameFromPath(path);
  MergeTagCatalogFromRegistry(scene_);
  SceneEntityInstance instance{};
  instance.instanceName = "Entity_0";
  instance.definition = MakeDefaultEntityDefinition("Entity_0");
  scene_.instances = {std::move(instance)};

  ui_.jsonFilePath = path;
  ui_.selectedScenePath = path;
  ui_.selectedInstanceIndex = 0;
  ui_.selectedComponentKind = ComponentSlotKind::Draw;
  ui_.selectedComponentIndex = 0;
  ui_.requestApplyPreview = true;
  ui_.requestSaveToDisk = true;
}

bool EntityEditor::OpenSceneFromPath(const std::string &sceneJsonPath) {
  if (sceneJsonPath.empty()) {
    return false;
  }
  ui_.jsonFilePath = sceneJsonPath;
  ui_.selectedScenePath = sceneJsonPath;
  LoadFromJsonPath(sceneJsonPath);
  ui_.requestApplyPreview = true;
  return true;
}

bool EntityEditor::DeleteSceneFile(const std::string &sceneJsonPath) {
  if (!Atrum::Editor::Scene::TryDeleteSceneJsonFile(sceneJsonPath)) {
    return false;
  }

  if (ui_.selectedScenePath == sceneJsonPath) {
    ui_.selectedScenePath.clear();
  }
  if (ui_.pendingDeleteScenePath == sceneJsonPath) {
    ui_.pendingDeleteScenePath.clear();
  }

  const bool wasOpen = ui_.jsonFilePath == sceneJsonPath;
  if (!wasOpen) {
    return true;
  }

  const std::vector<std::string> remaining =
      Atrum::Editor::Scene::ListSceneJsonFiles();
  if (!remaining.empty()) {
    return OpenSceneFromPath(remaining.front());
  }

  CreateNewSceneFile();
  return true;
}

void EntityEditor::ProcessDroppedFileImports() {
  const std::vector<std::string> dropped =
      Atrum::Editor::Import::FileDropQueue::Instance().Drain();
  if (dropped.empty()) {
    return;
  }

  Atrum::Editor::Import::EnsureImportedAssetsDirectoryExists();

  for (const std::string &sourcePath : dropped) {
    const std::string dest =
        Atrum::Editor::Import::CopyFileIntoImportedAssets(sourcePath);
    if (dest.empty()) {
      ui_.importStatusMessage = "インポート失敗: " + sourcePath;
      continue;
    }

    ui_.lastImportedAssetPath = dest;
    ui_.importStatusMessage = "Imported へコピー: " + dest;
  }
}

} // namespace Atrum::EntityEditor
