#include "System/Editor/EditorImportedAssetApplyToEntityDefinition.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <cctype>
#include <filesystem>

namespace Atrum::Editor::EditorImportedAssetApplyToEntityDefinition {

namespace {

std::string ExtensionLower(const std::string &path) {
  std::string ext = std::filesystem::path(path).extension().string();
  for (char &ch : ext) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return ext;
}

EntityEditor::ComponentSlot *FindSlot(EntityEditor::EntityDefinition &definition,
                                    const std::string &typeName) {
  for (EntityEditor::ComponentSlot &slot : definition.components) {
    if (slot.typeName == typeName) {
      return &slot;
    }
  }
  return nullptr;
}

EntityEditor::ComponentSlot &EmplaceSlot(
    EntityEditor::EntityDefinition &definition, const std::string &typeName,
    const EntityEditor::ComponentSlotKind kind) {
  if (EntityEditor::ComponentSlot *existing = FindSlot(definition, typeName)) {
    return *existing;
  }
  EntityEditor::ComponentSlot slot{};
  slot.kind = kind;
  slot.typeName = typeName;
  slot.params = EntityEditor::DefaultParamsForType(typeName);
  definition.components.push_back(std::move(slot));
  return definition.components.back();
}

} // namespace

bool IsSupportedAssetExtension(const std::string &importedPath) {
  const std::string ext = ExtensionLower(importedPath);
  return ext == ".obj" || ext == ".png" || ext == ".jpg" || ext == ".jpeg";
}

int ApplyImportedPathToAllMatchingSlots(
    EntityEditor::EntityDefinition &definition, const std::string &importedPath) {
  if (!IsSupportedAssetExtension(importedPath)) {
    return 0;
  }
  const std::string ext = ExtensionLower(importedPath);
  const bool isObj = ext == ".obj";
  const bool isImage = ext == ".png" || ext == ".jpg" || ext == ".jpeg";

  int updated = 0;
  for (EntityEditor::ComponentSlot &slot : definition.components) {
    if (slot.typeName == "CmpEditorAssetPath") {
      slot.params["assetPath"] = importedPath;
      slot.params["loadObjAsMesh"] = isObj;
      slot.params["loadImageAsBillboard"] = isImage;
      ++updated;
    } else if (isObj && slot.typeName == "CmpMesh") {
      slot.params["modelAssetPath"] = importedPath;
      ++updated;
    } else if (isImage && slot.typeName == "CmpEditorTextureBillboard") {
      slot.params["textureFilePath"] = importedPath;
      ++updated;
    } else if (isImage && slot.typeName == "CmpBillboardInstancedGrid") {
      slot.params["textureFilePath"] = importedPath;
      ++updated;
    } else if (isImage && slot.typeName == "CmpRuntimeTexturePath") {
      slot.params["texturePath"] = importedPath;
      ++updated;
    }
  }
  return updated;
}

bool ApplyImportedPath(EntityEditor::EntityDefinition &definition,
                       const std::string &importedPath) {
  if (!IsSupportedAssetExtension(importedPath)) {
    return false;
  }
  if (ApplyImportedPathToAllMatchingSlots(definition, importedPath) > 0) {
    return true;
  }
  const std::string ext = ExtensionLower(importedPath);
  const bool isObj = ext == ".obj";
  const bool isImage = ext == ".png" || ext == ".jpg" || ext == ".jpeg";

  EntityEditor::ComponentSlot &assetSlot = EmplaceSlot(
      definition, "CmpEditorAssetPath", EntityEditor::ComponentSlotKind::Data);
  assetSlot.params["assetPath"] = importedPath;
  assetSlot.params["loadObjAsMesh"] = isObj;
  assetSlot.params["loadImageAsBillboard"] = isImage;

  if (isObj) {
    EntityEditor::ComponentSlot &mesh =
        EmplaceSlot(definition, "CmpMesh", EntityEditor::ComponentSlotKind::Draw);
    mesh.params["modelAssetPath"] = importedPath;
  }
  if (isImage) {
    EntityEditor::ComponentSlot &billboard = EmplaceSlot(
        definition, "CmpEditorTextureBillboard",
        EntityEditor::ComponentSlotKind::Draw);
    billboard.params["textureFilePath"] = importedPath;
  }
  return true;
}

} // namespace Atrum::Editor::EditorImportedAssetApplyToEntityDefinition