#pragma once

#include "Scene/CommandSceneDevelopmentBoot.h"

namespace Atrum {

class Scene;

/// DEVELOPMENT 定義時は常にエディタシーン（SceneEntityEditor）で起動し、起動対象 JSON があれば初期編集対象として渡す。
Scene *ApplyDevelopmentEditorBootScene(const DevelopmentBootTarget &target);

} // namespace Atrum
