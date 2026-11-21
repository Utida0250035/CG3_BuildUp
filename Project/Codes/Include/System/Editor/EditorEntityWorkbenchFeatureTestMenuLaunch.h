#pragma once

namespace Atrum::EntityEditor {

struct UiState;

} // namespace Atrum::EntityEditor

namespace Atrum::Editor {

/// エディタメニュー等から FeatureTest 正本を SceneBuildUp で起動する。
void QueueFeatureVerificationSceneBuildUp(EntityEditor::UiState &ui);

} // namespace Atrum::Editor