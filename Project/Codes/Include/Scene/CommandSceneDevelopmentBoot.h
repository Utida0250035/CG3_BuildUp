#pragma once

#include <string>

namespace Atrum {

enum class DevelopmentBootKind {
  EntityEditorDefault,
  EntityEditorWithJson,
  SceneBuildUp,
};

struct DevelopmentBootTarget {
  DevelopmentBootKind kind = DevelopmentBootKind::EntityEditorDefault;
  std::string sceneJsonPath{};
};

/// EditorScenePlayRequest / launch_config から Development 起動先を決める
DevelopmentBootTarget ResolveDevelopmentBootTarget();

/// Release 起動用: パスが読める Scene JSON か検証する
bool IsValidBootSceneJsonPath(const std::string &sceneJsonPath);

} // namespace Atrum