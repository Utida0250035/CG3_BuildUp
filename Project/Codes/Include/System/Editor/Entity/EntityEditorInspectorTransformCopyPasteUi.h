#pragma once

#include "Math/Transform.h"

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorInspectorTransformCopyPasteUi {

/// 初期トランスフォームのコピー／貼り付け／複数選択へ一括適用
bool Draw(EntityEditor &editor, Math::TransformLH &initialTransform,
          bool &outChanged);

} // namespace Atrum::EntityEditor::EntityEditorInspectorTransformCopyPasteUi