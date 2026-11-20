#pragma once

namespace Atrum {

class Scene;

/// プレイヤー向け DebugCamera 入力（CmpCamera::Update）を許可するかの単一判定。
/// IsStageViewCameraLocked と AllowDebugCameraControl の組み合わせ正本。
bool AllowsPlayerDebugCameraInput(const Scene &scene);

} // namespace Atrum