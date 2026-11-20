#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include <string>

namespace Atrum::EntityEditor {

struct EntityEditorUi;

/// launch_config の Release 起動 JSON を BuildUp 起動リクエストへ載せる。
bool TryQueueBuildUpFromReleaseStartup(EntityEditorUi &ui, std::string *statusMessage);

} // namespace Atrum::EntityEditor

#endif