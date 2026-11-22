#pragma once

#include <string>

namespace Atrum::EntityEditor {
struct EntityDefinition;
}

namespace Atrum::Editor::EditorImportedAssetApplyToEntityDefinition {

bool IsSupportedAssetExtension(const std::string &importedPath);

/// 定義内の該当 typeName をすべて更新。一致スロット数を返す（0 なら未更新）
int ApplyImportedPathToAllMatchingSlots(EntityEditor::EntityDefinition &definition,
                                        const std::string &importedPath);

/// CmpEditorAssetPath と対応 Draw スロットへパスを書き込む（既存スロットは上書き）。
bool ApplyImportedPath(EntityEditor::EntityDefinition &definition,
                       const std::string &importedPath);

} // namespace Atrum::Editor::EditorImportedAssetApplyToEntityDefinition