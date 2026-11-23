#pragma once

#include "Scene/CommandSceneBootLaunchPathGate.h"
#include <string>

namespace Atrum {

enum class DevelopmentBootKind {
  EntityEditorDefault,
  EntityEditorWithJson,
  /// DebugPlayArea: BuildUp（エディタへ戻る）
  SceneBuildUp,
  /// Release と同じ Assets/Scenes 経路で BuildUp（Title へ戻る）
  SceneBuildUpPlayRuntime,
};

struct DevelopmentBootTarget {
  DevelopmentBootKind kind = DevelopmentBootKind::EntityEditorDefault;
  std::string sceneJsonPath{};
};

/// EditorScenePlayRequest / launch_config から Development 起動先を決める
DevelopmentBootTarget ResolveDevelopmentBootTarget();

} // namespace Atrum