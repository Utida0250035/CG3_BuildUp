#include "System/Scripting/LuaEntitySpawnFromJson.h"

#include "Scene/SceneBuildUpSceneDocumentTagCatalog.h"
#include "Scene/SceneBuildUpSceneJsonPathResolve.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"
#include "System/Entity/Entity.h"

#include <filesystem>
#include <string>
#include <utility>

namespace Atrum::Script {

namespace {

std::string CanonicalIfExists(const std::string &candidate) {
  if (candidate.empty()) {
    return {};
  }
  namespace fs = std::filesystem;
  std::error_code error{};
  if (!fs::exists(candidate, error) || error) {
    return {};
  }
  try {
    return fs::weakly_canonical(fs::path(candidate)).generic_string();
  } catch (...) {
    return candidate;
  }
}

void ApplyWorldOffset(EntityEditor::EditorSceneDocument &document, float offsetX,
                      float offsetY, float offsetZ) {
  if (offsetX == 0.0f && offsetY == 0.0f && offsetZ == 0.0f) {
    return;
  }
  for (EntityEditor::SceneEntityInstance &instance : document.instances) {
    if (instance.parentIndex < 0) {
      instance.definition.initialTransform.translate.x += offsetX;
      instance.definition.initialTransform.translate.y += offsetY;
      instance.definition.initialTransform.translate.z += offsetZ;
    }
  }
}

} // namespace

std::string ResolveLuaEntityJsonPath(const std::string &path) {
  if (path.empty()) {
    return {};
  }

  const std::string sceneResolved = ResolveSceneBuildUpJsonPath(path);
  if (!CanonicalIfExists(sceneResolved).empty()) {
    return sceneResolved;
  }
  if (!CanonicalIfExists(path).empty()) {
    return path;
  }

  namespace fs = std::filesystem;
  const fs::path inputPath(path);
  const std::string fileName = inputPath.filename().string();

  const std::string entityCandidate =
      std::string(Atrum::EditorAssets::kEntitiesPrefabDirectory) + "/" + path;
  if (const std::string resolved = CanonicalIfExists(entityCandidate);
      !resolved.empty()) {
    return resolved;
  }
  if (!fileName.empty() && fileName != path) {
    const std::string entityByName =
        std::string(Atrum::EditorAssets::kEntitiesPrefabDirectory) + "/" +
        fileName;
    if (const std::string resolved = CanonicalIfExists(entityByName);
        !resolved.empty()) {
      return resolved;
    }
  }

  return sceneResolved.empty() ? path : sceneResolved;
}

LuaEntitySpawnResult TryAppendEntitiesFromJsonPath(
    const std::string &jsonPath, std::vector<std::unique_ptr<Entity>> &entities,
    const float offsetX, const float offsetY, const float offsetZ) {
  LuaEntitySpawnResult result{};

  const std::string resolvedPath = ResolveLuaEntityJsonPath(jsonPath);
  EntityEditor::EditorSceneDocument document{};
  if (!EntityEditor::TryLoadSceneDocument(resolvedPath, document)) {
    result.message = "JSON の読込に失敗: " + resolvedPath;
    return result;
  }

  SceneBuildUpSceneDocumentTagCatalog::ApplyAfterJsonLoad(document);
  ApplyWorldOffset(document, offsetX, offsetY, offsetZ);

  EntityEditor::EditorSceneRuntimeBuildEntitiesOptions buildOptions{};
  buildOptions.addMainCameraIfMissing = false;

  std::vector<std::unique_ptr<Entity>> spawned =
      EntityEditor::BuildEntitiesFromSceneDocument(document, nullptr,
                                                   buildOptions);
  if (spawned.empty()) {
    result.message = "生成できる Entity がありません: " + resolvedPath;
    return result;
  }

  result.spawnedCount = static_cast<int>(spawned.size());
  for (std::unique_ptr<Entity> &entity : spawned) {
    entities.push_back(std::move(entity));
  }

  result.success = true;
  result.message =
      "spawned " + std::to_string(result.spawnedCount) + " from " + resolvedPath;
  return result;
}

} // namespace Atrum::Script