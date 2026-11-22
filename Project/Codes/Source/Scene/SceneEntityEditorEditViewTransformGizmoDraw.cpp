#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditorEditViewTransformGizmoDraw.h"

#include "Engine/AtrumEngine.h"
#include "Math/Vector4.h"
#include "Scene/SceneEntityEditor.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/WhiteTexture.h"

namespace Atrum::SceneEntityEditorEditViewTransformGizmoDraw {

namespace {

constexpr float kAxisLength = 1.25f;
constexpr float kLineWidth = 0.035f;

void DrawAxisLine(AtrumEngine *engine, const Math::Vector3 &origin,
                  const Math::Vector3 &end, const Math::Vector4 &color) {
  if (engine == nullptr) {
    return;
  }
  engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{}, origin, end,
                     kLineWidth, false);
}

} // namespace

void Draw(AtrumEngine *engine, SceneEntityEditor &scene) {
  EntityEditor::EntityEditor *editor = scene.MutableEntityEditor();
  if (editor == nullptr) {
    return;
  }
  const EntityEditor::UiState &ui = editor->Ui();
  if (ui.interactionMode != EntityEditor::EditorInteractionMode::Edit) {
    return;
  }

  const EntityEditor::EditorSceneDocument &document = editor->SceneDocument();
  if (document.instances.empty()) {
    return;
  }
  const size_t index = static_cast<size_t>(ui.selectedInstanceIndex);
  if (index >= document.instances.size()) {
    return;
  }

  const Math::Vector3 origin =
      document.instances[index].definition.initialTransform.translate;

  DrawAxisLine(engine, origin, origin + Math::Vector3{kAxisLength, 0.0f, 0.0f},
               {0.95f, 0.32f, 0.32f, 0.95f});
  DrawAxisLine(engine, origin,
               origin + Math::Vector3{0.0f, kAxisLength, 0.0f},
               {0.35f, 0.9f, 0.4f, 0.95f});
  DrawAxisLine(engine, origin, origin + Math::Vector3{0.0f, 0.0f, kAxisLength},
               {0.38f, 0.58f, 0.98f, 0.95f});
}

} // namespace Atrum::SceneEntityEditorEditViewTransformGizmoDraw

#else

namespace Atrum::SceneEntityEditorEditViewTransformGizmoDraw {

void Draw(AtrumEngine *, SceneEntityEditor &) {}

} // namespace Atrum::SceneEntityEditorEditViewTransformGizmoDraw

#endif