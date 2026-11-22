#pragma once

#include <string>

namespace Atrum::Editor {

/// png / jpg / obj / gltf / glb など外部編集向けの拡張子か
bool IsExternalAssetEditorTargetExtension(const std::string &path);

/// プロジェクト相対パスを cwd 基準で実ファイルに解決
std::string ResolveExistingProjectAssetFile(const std::string &path);

/// OS の既定アプリでファイルを開く。失敗時は outError に理由
bool TryOpenExternalAssetWithDefaultApp(const std::string &projectRelativePath,
                                        std::string &outError);

} // namespace Atrum::Editor