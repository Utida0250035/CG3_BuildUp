#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum {

class AtrumEngine;
class Camera;

namespace SceneEntityEditorSceneViewRenderCameraBind {

/// GameOrder で Bind 済みのカメラを RT サイズに Fit し、ActiveCameraSystem 経由で engine に載せる。
void BindForSceneViewRender(AtrumEngine *engine, Camera *sceneCamera, int width,
                            int height);

} // namespace SceneEntityEditorSceneViewRenderCameraBind

} // namespace Atrum

#endif