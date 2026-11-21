#include "System/Editor/Entity/EntityEditorDefaultParamsCmpCamera.h"

namespace Atrum::EntityEditor::EntityEditorDefaultParamsCmpCamera {

nlohmann::json Json() {
  return {{"perspectiveFovY", 0.5f},
          {"nearClip", 0.125f},
          {"farClip", 128.0f}};
}

} // namespace Atrum::EntityEditor::EntityEditorDefaultParamsCmpCamera