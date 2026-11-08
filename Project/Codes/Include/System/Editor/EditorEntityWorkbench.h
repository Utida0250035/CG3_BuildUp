#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::Editor {

/// Unity 風のドック分割シェル＋シーンビュー矩形の計測
void LayoutEntityEditorWorkbench(EntityEditor::EntityEditor &editor);

} // namespace Atrum::Editor

#endif
