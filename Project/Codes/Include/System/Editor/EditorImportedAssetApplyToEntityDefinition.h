#pragma once

#include <string>

namespace Atrum::EntityEditor {
struct EntityDefinition;
}

namespace Atrum::Editor::EditorImportedAssetApplyToEntityDefinition {

bool IsSupportedAssetExtension(const std::string &importedPath);

/// CmpEditorAssetPath と対応 Draw スロットへパスを書き込む（既存スロットは上書き）。
bool ApplyImportedPath(EntityDefinition &definition,
                       const std::string &importedPath);

} // namespace Atrum::Editor::EditorImportedAssetApplyToEntityDefinition