#pragma once

namespace Atrum::SceneBuildUpChangeSceneDrawPolicy {

/// フェード等の遷移演出が実行中のときだけ CommandChangeScene を描画する
void DrawTransitionWhenActive();

} // namespace Atrum::SceneBuildUpChangeSceneDrawPolicy