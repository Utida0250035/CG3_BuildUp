#include "System/Editor/Scene/EditorSceneAsset.h"

#include "System/Editor/EditorLaunchConfigStartupPathNormalize.h"
#include "System/Editor/EditorScenePlayRequestMetadata.h"

namespace Atrum::Editor::Scene {

namespace {

std::string NormalizeStagedPath(const std::string &path) {
  return LaunchConfigStartupPathNormalize::NormalizeForLaunchConfig(path);
}

} // namespace

EditorScenePlayRequest &EditorScenePlayRequest::Instance() {
  static EditorScenePlayRequest instance;
  return instance;
}

void EditorScenePlayRequest::Stage(const std::string &jsonFilePath,
                                   const EditorScenePlayStagingKind kind) {
  if (jsonFilePath.empty()) {
    Clear();
    return;
  }
  jsonFilePath_ = NormalizeStagedPath(jsonFilePath);
  stagingKind_ = kind;
}

void EditorScenePlayRequest::Set(const std::string &jsonFilePath) {
  Stage(jsonFilePath, EditorScenePlayStagingKind::Unset);
}

void EditorScenePlayRequest::Clear() {
  jsonFilePath_.clear();
  stagingKind_ = EditorScenePlayStagingKind::Unset;
}

EditorScenePlayStagingKind EditorScenePlayRequest::StagingKind() const {
  return stagingKind_;
}

void StageEditorPlayScenePath(const std::string &jsonFilePath,
                              const EditorScenePlayStagingKind kind) {
  EditorScenePlayRequest::Instance().Stage(jsonFilePath, kind);
}

std::string TakePendingEditorPlayScenePath() {
  const std::string &pending = EditorScenePlayRequest::Instance().Path();
  if (pending.empty()) {
    return {};
  }
  EditorScenePlayRequest::Instance().Clear();
  return pending;
}

void ClearStagedEditorPlayScenePath() {
  EditorScenePlayRequest::Instance().Clear();
}

} // namespace Atrum::Editor::Scene