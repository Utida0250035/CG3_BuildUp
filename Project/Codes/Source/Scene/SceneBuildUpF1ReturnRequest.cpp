#include "Scene/SceneBuildUpF1ReturnRequest.h"

#include "Engine/EngineAccess.h"
#include "Input/PlayInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneBuildUpReturnTarget.h"

#include <memory>

namespace Atrum::SceneBuildUpF1ReturnRequest {

bool TryRequestReturnViaF1(SceneBuildUp &scene) {
  if (!::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F1)) {
    return false;
  }
  Scene *const returnTarget =
      ResolveSceneBuildUpReturnTarget(scene.ReturnScene());
  Atrum::CommandChangeScene::Instance()->Set(
      returnTarget, std::make_unique<ChangeSceneFade>().release());
  return true;
}

} // namespace Atrum::SceneBuildUpF1ReturnRequest