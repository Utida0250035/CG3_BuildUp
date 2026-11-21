#include "System/Editor/Entity/EntityEditorSceneObjectAddPreset.h"

#include "System/Editor/EditorAssetsPaths.h"
#include "System/Editor/Entity/EntityEditorComponentSlotParamsNormalize.h"

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace Atrum::EntityEditor {

namespace fs = std::filesystem;

namespace {

EntityDefinition MakeMinimalEmptyEntityDefinition(const std::string &entityName) {
  EntityDefinition definition{};
  definition.entityName = entityName;
  definition.components.clear();
  definition.tags.clear();
  definition.tagHashes.clear();
  return definition;
}

std::optional<EntityDefinition>
TryLoadMainCameraFromEditorAssets(const std::string &entityName) {
  const fs::path path =
      fs::path(EditorAssets::kEntitiesPrefabDirectory) / "MainCamera.json";
  std::ifstream stream(path);
  if (!stream.is_open()) {
    return std::nullopt;
  }
  nlohmann::json json{};
  try {
    stream >> json;
  } catch (...) {
    return std::nullopt;
  }
  EntityDefinition definition = json.get<EntityDefinition>();
  definition.entityName = entityName;
  for (ComponentSlot &slot : definition.components) {
    NormalizeKnownComponentSlotParams(slot);
  }
  return definition;
}

} // namespace

std::optional<EntityDefinition>
BuildEntityDefinitionForAddPreset(const SceneObjectAddPreset preset,
                                  const std::string &entityName) {
  switch (preset) {
  case SceneObjectAddPreset::Empty:
    return MakeMinimalEmptyEntityDefinition(entityName);
  case SceneObjectAddPreset::StarterSample:
    return MakeDefaultEntityDefinition(entityName);
  case SceneObjectAddPreset::MainCamera:
    return TryLoadMainCameraFromEditorAssets(entityName);
  case SceneObjectAddPreset::CopySelection:
    return std::nullopt;
  }
  return std::nullopt;
}

} // namespace Atrum::EntityEditor