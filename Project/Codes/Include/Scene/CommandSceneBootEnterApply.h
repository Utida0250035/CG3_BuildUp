#pragma once

namespace Atrum {

class Scene;

/// sceneNow 確定後の初回 Enter 前に呼ぶ（Release 起動 BuildUp は JSON 検証し Title へ差し替え可能）
Scene *PrepareInitialSceneBeforeEnter(Scene *sceneNow);

/// 初回シーン進入（sceneNow は Prepare 済みを渡す）
void EnterInitialSceneNow(Scene *sceneNow);

} // namespace Atrum