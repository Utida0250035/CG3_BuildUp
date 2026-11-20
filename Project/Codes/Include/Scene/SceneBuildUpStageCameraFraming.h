#pragma once

namespace Atrum {

class Scene;

namespace EntityEditor {
struct EditorSceneDocument;
}

/// BuildUp ランタイムでシーン JSON の StageView 境界をカメラへ反映する
namespace SceneBuildUpStageCamera {

void ApplyFramingFromSceneDocument(Scene &scene,
                                   const EntityEditor::EditorSceneDocument
                                       &document);

void MaintainLockedStageCamera(Scene &scene);

} // namespace SceneBuildUpStageCamera

} // namespace Atrum