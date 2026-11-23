#pragma once

namespace Atrum::EntityEditor {
struct ComponentSlot;
}

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsHitSphere {

/// CmpHitSphere のインスペクタ（複数球の選択・一括編集・複製・一括削除に対応）
bool TryDrawCmpHitSphereComponentParams(ComponentSlot &slot, bool &outChanged);

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsHitSphere
