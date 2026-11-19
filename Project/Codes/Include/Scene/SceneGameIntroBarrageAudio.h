#pragma once

namespace Atrum {

class Entity;

/// イントロ中: 軌道のみ＋ボス Update（弾幕 SE）。自機入力・当たり解決は行わない
void UpdateSceneGameIntroBarrageAudio(Entity *boss);

} // namespace Atrum