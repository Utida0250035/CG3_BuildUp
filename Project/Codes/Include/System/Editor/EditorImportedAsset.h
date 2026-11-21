#pragma once

#include <string>
#include <vector>

namespace Atrum::Editor::Import {

/// ウィンドウへドロップした外部ファイルのコピー先（ゲーム本番では未使用）
constexpr const char *kImportedAssetsDirectory = "../EditorAssets/Imported";

void EnsureImportedAssetsDirectoryExists();

/// ソースファイルを Imported へコピー。成功時はコピー先の相対パス、失敗時は空文字。
std::string CopyFileIntoImportedAssets(const std::string &sourcePath);

/// `kImportedAssetsDirectory` 直下のファイルを相対パスで列挙（名前昇順・非再帰）。
std::vector<std::string> ListImportedAssetRelativePaths();

/// Imported 管理下のファイルのみ削除。
bool TryDeleteImportedAssetFile(const std::string &relativePath);

/// 新しいファイル名（ベース名のみ）。成功時は新しい相対パス。
std::string TryRenameImportedAssetFile(const std::string &relativePath,
                                       const std::string &newFileName);

/// SDL_DROPFILE からエンジンが受け取ったパス（メインスレッド専用）
class FileDropQueue {
public:
  static FileDropQueue &Instance();

  void Push(std::string path);
  std::vector<std::string> Drain();

private:
  FileDropQueue() = default;
  std::vector<std::string> pending_;
};

} // namespace Atrum::Editor::Import
