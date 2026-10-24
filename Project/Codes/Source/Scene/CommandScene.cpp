#include "Scene/CommandScene.h"
#include "Scene/SceneTitle.h"

namespace Atrum {

/// <summary>
/// 指令類 > シーン指令 > コンストラクタ
/// </summary>
CommandScene::CommandScene() {

  // 現在シーンにタイトルシーンを設定
  sceneNow_ = SceneTitle::Instance();

  // 現在シーンの進入処理
  sceneNow_->EnterScene();
}

} // namespace Atrum