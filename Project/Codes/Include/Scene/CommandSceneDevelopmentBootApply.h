#pragma once

#include "Scene/CommandSceneDevelopmentBoot.h"

namespace Atrum {

class Scene;

/// Development 起動ターゲットを初期 sceneNow に割り当てる。
Scene *CreateInitialSceneForDevelopmentBoot(const DevelopmentBootTarget &target);

} // namespace Atrum