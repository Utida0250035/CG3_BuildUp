#pragma once

namespace Atrum {

class SceneEntityEditor;

namespace EntityEditor {
class EntityEditor;
}

/// プレビューに primary CmpCamera が無いとき StageView ポーズへフォールバックする。
void ApplyPreviewStageViewCameraFallback(
    SceneEntityEditor &scene, const EntityEditor::EntityEditor &editor);

} // namespace Atrum