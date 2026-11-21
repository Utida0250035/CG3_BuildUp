#pragma once

#include "Math/Transform.h"

#include <optional>
#include <vector>

namespace Atrum::EntityEditor {
class EntityEditor;
struct UiState;
}

namespace Atrum::EntityEditor::EntityEditorTransformCopyPaste {

void CopyInitialTransformToClipboard(UiState &ui,
                                     const Math::TransformLH &transform);

bool PasteInitialTransformFromClipboard(UiState &ui,
                                        Math::TransformLH &transform);

/// クリップボードの初期トランスフォームを複数インスタンスへ適用。適用件数
int ApplyClipboardTransformToInstanceIndices(
    EntityEditor &editor, const std::vector<int> &indices);

} // namespace Atrum::EntityEditor::EntityEditorTransformCopyPaste