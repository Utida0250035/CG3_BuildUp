#pragma once

#include "System/Editor/EditorAssetsPaths.h"
#include <string>
#include <vector>

namespace Atrum::Editor::Scene {

constexpr const char *kScenesDirectory = EditorAssets::kScenesDirectory;

/// シーンフォルダを用意する（存在しなければ作成）
void EnsureScenesDirectoryExists();

/// フォルダ内の *.json シーンパス一覧（相対パス）
std::vector<std::string> ListSceneJsonFiles(
    const std::string &directoryRelativePath = kScenesDirectory);

/// 未使用のシーンファイルパスを生成（Scene_0.json 形式）
std::string MakeUniqueSceneFilePath();

/// パスからシーン表示名（拡張子なしファイル名）を得る
std::string SceneDisplayNameFromPath(const std::string &jsonFilePath);

/// Scenes フォルダ内のシーン JSON を削除する（成功で true）
bool TryDeleteSceneJsonFile(const std::string &jsonFilePath);

/// 実行シーン遷移用の JSON パス（SceneEntityEditor → Runtime）
class EditorScenePlayRequest {
public:
  static EditorScenePlayRequest &Instance();

  void Set(const std::string &jsonFilePath);
  const std::string &Path() const { return jsonFilePath_; }
  void Clear();

private:
  std::string jsonFilePath_{};
};

} // namespace Atrum::Editor::Scene
