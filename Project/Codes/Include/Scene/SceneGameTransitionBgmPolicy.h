#pragma once

namespace Atrum {

class SceneGame;

/// ChangeSceneFade 実行中: PlayBgmLoop 再発火なしの BGM 維持のみ
void MaintainSceneGameBgmDuringTransitionEffect(SceneGame &scene);

} // namespace Atrum