#pragma once

#include <string>

namespace Atrum::EntityEditor {
enum class ImportedAssetNameCollisionPolicy;
}

namespace Atrum::Editor::Import {

/// 同名ファイルが Imported に既にあるときの保存先ファイル名を決める。
std::string ResolveImportedAssetDestinationFileName(
    const std::string &sourceFileName, bool destinationExists,
    EntityEditor::ImportedAssetNameCollisionPolicy policy,
    const std::string &batchNamePrefix);

} // namespace Atrum::Editor::Import