#pragma once

namespace Atrum::SceneBuildUpRuntimeLightingPolicy {

/// ランタイムビジュアル（ライト等）は EnterScene で一度だけ適用する。
/// lightModel 含む DirectionalLight は game_scene_lighting.json 経由で一度だけ反映し、
/// Draw / DrawRuntimeViewContent で HalfLambert 等を再固定しない。

void ApplyOnceOnEnterScene();

} // namespace Atrum::SceneBuildUpRuntimeLightingPolicy