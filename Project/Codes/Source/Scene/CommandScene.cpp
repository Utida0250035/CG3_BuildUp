#include "Scene/CommandScene.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/SceneEntityEditor.h"
#else
#include "Scene/SceneTitle.h"
#endif

namespace Atrum {

/// <summary>
/// 指令類 > シーン指令 > コンストラクタ
/// </summary>
CommandScene::CommandScene() {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  sceneNow_ = SceneEntityEditor::Instance();
#else
  sceneNow_ = SceneTitle::Instance();
#endif

  sceneNow_->EnterScene();
}

} // namespace Atrum