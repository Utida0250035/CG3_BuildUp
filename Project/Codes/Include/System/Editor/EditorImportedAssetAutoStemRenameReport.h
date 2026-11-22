#pragma once

#include <string>

namespace Atrum::Editor::Import {

/// コピー先がソースと異なり、stem_数字 形式なら自動 stem 連番リネームとみなす。
bool UsedAutoStemIncrementRename(const std::string &sourcePath,
                                 const std::string &destRelativePath);

/// 1 件分のインポート結果説明（自動リネームを明示）。
std::string DescribeImportedCopyLine(const std::string &sourcePath,
                                     const std::string &destRelativePath);

} // namespace Atrum::Editor::Import