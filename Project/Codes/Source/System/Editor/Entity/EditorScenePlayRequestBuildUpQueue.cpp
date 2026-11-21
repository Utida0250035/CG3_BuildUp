#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/Entity/EditorScenePlayRequestBuildUpQueue.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorStartupSceneSetWithoutAutosave.h"

namespace Atrum::EntityEditor {

bool TrySetStartupSceneAndQueueBuildUp(EntityEditor &editor,
                                        std::string *statusMessage) {
  return EntityEditorStartupSceneSetWithoutAutosave::SetAndQueueBuildUp(
      editor, statusMessage);
}

} // namespace Atrum::EntityEditor

#endif