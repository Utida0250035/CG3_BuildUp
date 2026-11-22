#include "System/Editor/Entity/EntityEditorDroppedImportSpawnSceneInstances.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

#include <cctype>
#include <filesystem>

namespace Atrum::EntityEditor::EntityEditorDroppedImportSpawnSceneInstances {

namespace {

std::string ExtensionLower(const std::string &path) {
  std::string ext = std::filesystem::path(path).extension().string();
  for (char &ch : ext) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return ext;
}

std::string StemFromPath(const std::string &path) {
  return std::filesystem::path(path).stem().string();
}

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
    baseName = "Imported";
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

ComponentSlot MakeAssetPathSlot(const std::string &assetPath, const bool loadObj,
                                const bool loadImage) {
  ComponentSlot slot{};
  slot.kind = ComponentSlotKind::Data;
  slot.typeName = "CmpEditorAssetPath";
  slot.params = DefaultParamsForType("CmpEditorAssetPath");
  slot.params["assetPath"] = assetPath;
  slot.params["loadObjAsMesh"] = loadObj;
  slot.params["loadImageAsBillboard"] = loadImage;
  return slot;
}

bool IsSupportedImportedAsset(const std::string &path) {
  const std::string ext = ExtensionLower(path);
  return ext == ".obj" || ext == ".png" || ext == ".jpg" || ext == ".jpeg";
}

void BuildInstanceDefinition(const std::string &importedPath,
                             const std::string &instanceName,
                             EntityDefinition &outDefinition) {
  const std::string ext = ExtensionLower(importedPath);
  const bool isObj = ext == ".obj";
  const bool isImage = ext == ".png" || ext == ".jpg" || ext == ".jpeg";

  outDefinition = MakeDefaultEntityDefinition(instanceName);
  outDefinition.entityName = instanceName;
  outDefinition.components.clear();
  outDefinition.components.push_back(
      MakeAssetPathSlot(importedPath, isObj, isImage));

  if (isObj) {
    ComponentSlot mesh{};
    mesh.kind = ComponentSlotKind::Draw;
    mesh.typeName = "CmpMesh";
    mesh.params = DefaultParamsForType("CmpMesh");
    mesh.params["modelAssetPath"] = importedPath;
    outDefinition.components.push_back(std::move(mesh));
  }
  if (isImage) {
    ComponentSlot billboard{};
    billboard.kind = ComponentSlotKind::Draw;
    billboard.typeName = "CmpEditorTextureBillboard";
    billboard.params = DefaultParamsForType("CmpEditorTextureBillboard");
    billboard.params["textureFilePath"] = importedPath;
    outDefinition.components.push_back(std::move(billboard));
  }
}

} // namespace

int AppendFromImportedAssetPaths(EntityEditor &editor,
                                 const std::vector<std::string> &importedPaths) {
  if (importedPaths.empty()) {
    return 0;
  }

  EditorSceneDocument &scene = editor.SceneDocument();
  int spawned = 0;
  float rowOffset = 0.0f;

  for (const std::string &path : importedPaths) {
    if (!IsSupportedImportedAsset(path)) {
      continue;
    }

    SceneEntityInstance instance{};
    const std::string instanceName =
        MakeUniqueInstanceName(scene, StemFromPath(path));
    instance.instanceName = instanceName;
    instance.parentIndex = -1;
    BuildInstanceDefinition(path, instanceName, instance.definition);
    instance.definition.initialTransform.translate.x = rowOffset;
    rowOffset += 1.25f;

    scene.instances.push_back(std::move(instance));
    ++spawned;
  }

  if (spawned > 0) {
    UiState &ui = editor.Ui();
    ui.selectedInstanceIndex = static_cast<int>(scene.instances.size()) - 1;
    ui.selectedInstanceIndices = {ui.selectedInstanceIndex};
    editor.ClampSelectedInstanceIndex();
    EntityEditorAuthoringDocumentEditNotify::Notify(editor);
  }

  return spawned;
}

} // namespace Atrum::EntityEditor::EntityEditorDroppedImportSpawnSceneInstances