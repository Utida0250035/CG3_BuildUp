#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"
#include "System/Editor/Entity/EntityEditorBuildPreviewSceneBuildUpParity.h"
#include "System/Editor/Entity/EntityEditorSceneDocumentLoadFromPath.h"
#include "System/Editor/Entity/EntityEditorDroppedImportSpawnSceneInstances.h"
#include "System/Editor/Entity/EntityEditorPrefabSpawnFirstInstanceRoot.h"
#include "System/Editor/Entity/EntityEditorAddEmptyInstanceFromSelection.h"
#include "System/Editor/Entity/EntityEditorRemoveSelectedInstances.h"
#include "System/Editor/Entity/EntityEditorTickPendingRequests.h"
#include "System/Editor/Entity/EntityEditorRestoreSelectionAfterSceneLoad.h"
#include "System/Editor/Entity/EntityEditorDeleteSceneOpenSuccessor.h"

#include "System/Editor/EditorImportedAsset.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/Editor/Entity/EntityEditorDuplicateSelectedInstance.h"
#include "System/Editor/Entity/EntityEditorTagCatalog.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"
#include "System/Editor/Scene/EditorSceneRuntimeBuildWarnings.h"
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
    ui_.selectedInstanceIndices.clear();
    return;
  }
  const int maxIndex = static_cast<int>(scene_.instances.size()) - 1;
  ui_.selectedInstanceIndex =
      std::clamp(ui_.selectedInstanceIndex, 0, maxIndex);
  if (ui_.selectedInstanceIndices.empty()) {
    ui_.selectedInstanceIndices.push_back(ui_.selectedInstanceIndex);
  }
  ui_.selectedInstanceIndices.erase(
      std::remove_if(ui_.selectedInstanceIndices.begin(),
                     ui_.selectedInstanceIndices.end(),
                     [maxIndex](const int index) {
                       return index < 0 || index > maxIndex;
                     }),
      ui_.selectedInstanceIndices.end());
  if (ui_.selectedInstanceIndices.empty()) {
    ui_.selectedInstanceIndices.push_back(ui_.selectedInstanceIndex);
  }
  std::sort(ui_.selectedInstanceIndices.begin(),
            ui_.selectedInstanceIndices.end());
  ui_.selectedInstanceIndices.erase(
      std::unique(ui_.selectedInstanceIndices.begin(),
                  ui_.selectedInstanceIndices.end()),
      ui_.selectedInstanceIndices.end());
  ui_.selectedInstanceIndex = ui_.selectedInstanceIndices.front();
}

void EntityEditor::AddEmptyInstance() {
  EntityEditorAddEmptyInstanceFromSelection::AppendInstance(*this);
}

bool EntityEditor::DuplicateSelectedInstance() {
  return DuplicateSelectedInstanceInScene(*this);
}

void EntityEditor::RemoveSelectedInstance() {
  EntityEditorRemoveSelectedInstances::RemoveFromScene(*this);
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
  return EntityEditorPrefabSpawnFirstInstanceRoot::AppendFromPrefabFile(*this,
                                                                        prefabPath);
}

void EntityEditor::ResetSelectionAfterSceneDocumentReload() {
  ui_.selectedInstanceIndex = 0;
  ui_.selectedInstanceIndices = {0};
  ui_.selectedComponentIndex = 0;
  if (!SelectedDefinition().components.empty()) {
    ui_.selectedComponentKind =
        SelectedDefinition().components.front().kind;
  } else {
    ui_.selectedComponentKind = ComponentSlotKind::Draw;
  }
  ClampSelectedInstanceIndex();
}

void EntityEditor::ApplySceneDocumentLoadedFromDisk(
    EditorSceneDocument document) {
  const EntityEditorRestoreSelectionAfterSceneLoad::SelectionSnapshot snapshot =
      EntityEditorRestoreSelectionAfterSceneLoad::Capture(*this);
  scene_ = std::move(document);
  MergeTagCatalogFromRegistry(scene_);
  SyncAllEntityDefinitionTags(scene_);
  SyncTagCatalogFromInstances(scene_);
  EntityEditorRestoreSelectionAfterSceneLoad::Apply(*this, snapshot);
  ClearSceneDocumentDirty();
}

void EntityEditor::ApplyEmptySceneForMissingJsonPath(
    const std::string &jsonFilePath) {
  const EntityEditorRestoreSelectionAfterSceneLoad::SelectionSnapshot snapshot =
      EntityEditorRestoreSelectionAfterSceneLoad::Capture(*this);
  scene_ = EditorSceneDocument{};
  scene_.sceneName =
      Atrum::Editor::Scene::SceneDisplayNameFromPath(jsonFilePath);
  if (scene_.sceneName.empty()) {
    scene_.sceneName = "EditorScene";
  }
  SceneEntityInstance instance{};
  instance.instanceName = "Entity_0";
  instance.definition = MakeDefaultEntityDefinition("Entity_0");
  scene_.instances = {std::move(instance)};
  MergeTagCatalogFromRegistry(scene_);
  SyncAllEntityDefinitionTags(scene_);
  SyncTagCatalogFromInstances(scene_);
  EntityEditorRestoreSelectionAfterSceneLoad::Apply(*this, snapshot);
  ClearSceneDocumentDirty();
}

void EntityEditor::LoadFromJsonPath(const std::string &jsonFilePath) {
  std::string status;
  if (!EntityEditorSceneDocumentLoadFromPath::TryLoad(*this, jsonFilePath,
                                                     status)) {
    if (!status.empty()) {
      ui_.importStatusMessage = std::move(status);
    }
  } else if (!ui_.importStatusMessage.empty() &&
             ui_.importStatusMessage.find("シーン JSON") != std::string::npos) {
    ui_.importStatusMessage.clear();
  }
}

void EntityEditor::SaveToJsonPath(const std::string &jsonFilePath) {
  for (SceneEntityInstance &instance : scene_.instances) {
    RebuildEntityDefinitionTagHashes(instance.definition);
  }
  Json::JsonHandler::SaveToFile(jsonFilePath, scene_);
  ClearSceneDocumentDirty();
}

bool EntityEditor::TickPendingRequests() {
  return EntityEditorTickPendingRequests::Process(*this);
}

std::vector<std::unique_ptr<Entity>> EntityEditor::BuildPreviewScene() {
  return EntityEditorBuildPreviewSceneBuildUpParity::Build(
      scene_, ui_, &ui_.importStatusMessage);
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
  ui_.selectedInstanceIndices = {0};
  ui_.selectedComponentKind = ComponentSlotKind::Draw;
  ui_.selectedComponentIndex = 0;
  EntityEditorAuthoringDocumentEditNotify::Notify(*this);
}

bool EntityEditor::TryOpenSceneFromPath(const std::string &sceneJsonPath) {
  if (sceneJsonPath.empty()) {
    return false;
  }
  if (sceneJsonPath == ui_.jsonFilePath) {
    ui_.selectedScenePath = sceneJsonPath;
    return true;
  }
  if (sceneDocumentDirty_) {
    ui_.pendingSceneSwitchPath = sceneJsonPath;
    ui_.requestSceneSwitchUnsavedPrompt = true;
    return false;
  }
  return OpenSceneFromPath(sceneJsonPath);
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
  const std::vector<std::string> scenesBeforeDelete =
      Atrum::Editor::Scene::ListEditableSceneJsonFiles();
  const bool wasOpen = ui_.jsonFilePath == sceneJsonPath;

  if (!Atrum::Editor::Scene::TryDeleteSceneJsonFile(sceneJsonPath)) {
    return false;
  }

  if (ui_.selectedScenePath == sceneJsonPath) {
    ui_.selectedScenePath.clear();
  }
  if (ui_.pendingDeleteScenePath == sceneJsonPath) {
    ui_.pendingDeleteScenePath.clear();
  }

  if (!wasOpen) {
    return true;
  }

  const std::optional<std::string> successor =
      EntityEditorDeleteSceneOpenSuccessor::ResolveAfterDelete(
          scenesBeforeDelete, sceneJsonPath);
  if (successor.has_value()) {
    return OpenSceneFromPath(*successor);
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

  std::vector<std::string> importedPaths;
  importedPaths.reserve(dropped.size());

  for (const std::string &sourcePath : dropped) {
    const std::string dest =
        Atrum::Editor::Import::CopyFileIntoImportedAssets(sourcePath);
    if (dest.empty()) {
      ui_.importStatusMessage = "インポート失敗: " + sourcePath;
      continue;
    }

    ui_.lastImportedAssetPath = dest;
    importedPaths.push_back(dest);
  }

  const int spawned =
      EntityEditorDroppedImportSpawnSceneInstances::AppendFromImportedAssetPaths(
          *this, importedPaths);
  if (spawned > 0) {
    ui_.importStatusMessage =
        "Imported へ " + std::to_string(importedPaths.size()) +
        " 件コピーし、シーンに " + std::to_string(spawned) +
        " 体を追加しました";
  } else if (!importedPaths.empty()) {
    ui_.importStatusMessage =
        "Imported へコピー: " + importedPaths.back() +
        "（対応拡張子のみシーン追加: obj/png/jpg）";
  }
}

} // namespace Atrum::EntityEditor
