#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include <string>

namespace Atrum::EntityEditor {

struct EntityEditorUi;

/// PlayRequest::Set（起動シーン保存）直後に BuildUp 起動を予約する。
bool TrySetStartupSceneAndQueueBuildUp(EntityEditorUi &ui,
                                        const std::string &sceneJsonPath,
                                        std::string *statusMessage);

} // namespace Atrum::EntityEditor

#endif