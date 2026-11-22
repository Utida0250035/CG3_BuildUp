#pragma once

namespace Atrum {

class SceneBuildUp;

namespace SceneBuildUpAtomFCombatCamera {

/// atomF シーンならフレーミングと初回カメラを適用し true を返す。
bool ApplyOnEnterIfAtomFScene(SceneBuildUp &scene);

/// atomF なら自機追従の斜め見下ろし。それ以外は従来の MaintainLockedStageCamera。
void MaintainSceneCamera(SceneBuildUp &scene);

} // namespace SceneBuildUpAtomFCombatCamera

} // namespace Atrum