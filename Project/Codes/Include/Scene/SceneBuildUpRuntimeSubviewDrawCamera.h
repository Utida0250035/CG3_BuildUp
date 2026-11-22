#pragma once

namespace Atrum {

class Camera;
class Scene;

/// RT サブビュー描画用。GameOrder の Prepare 後に SetupStageViewCamera を再実行しない。
namespace SceneBuildUpRuntimeSubviewDrawCamera {

Camera *BindAfterGameOrderPrepare(Scene &scene, int viewportWidth, int viewportHeight);

} // namespace SceneBuildUpRuntimeSubviewDrawCamera

} // namespace Atrum