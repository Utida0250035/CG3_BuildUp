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

/// BuildEntities 直後（EnterScene）に StageView 姿勢だけ確定する
void ApplyStageCameraAfterSceneLoad(Scene &scene);

void MaintainLockedStageCamera(Scene &scene);

} // namespace SceneBuildUpStageCamera

} // namespace Atrum