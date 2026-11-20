#pragma once

namespace Atrum::SceneBuildUpRuntimeLightingPolicy {

/// ランタイムビジュアル（ライト等）は EnterScene で一度だけ適用する。
/// Draw / DrawRuntimeViewContent では再適用しない。

void ApplyOnceOnEnterScene();

} // namespace Atrum::SceneBuildUpRuntimeLightingPolicy