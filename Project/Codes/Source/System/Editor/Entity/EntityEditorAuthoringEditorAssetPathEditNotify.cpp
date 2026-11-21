#include "System/Editor/Entity/EntityEditorAuthoringEditorAssetPathEditNotify.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

namespace Atrum::EntityEditor::EntityEditorAuthoringEditorAssetPathEditNotify {

void NotifyEditorAssetPathPreviewRefresh(EntityEditor &editor) {
  EntityEditorAuthoringDocumentEditNotify::NotifyPreviewOnly(editor);
}

} // namespace Atrum::EntityEditor::EntityEditorAuthoringEditorAssetPathEditNotify