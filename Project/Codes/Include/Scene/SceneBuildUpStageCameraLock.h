#pragma once

namespace Atrum {

class SceneBuildUp;

/// 機能検証 BuildUp の StageView ロック（既定 ON、Development で切替可）
namespace SceneBuildUpStageCameraLock {

constexpr bool kDefaultLocked = true;

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
/// F5 でロック ON/OFF。押下時 true を返す。
bool TryToggleLockHotkey(SceneBuildUp &scene);
#endif

} // namespace SceneBuildUpStageCameraLock

} // namespace Atrum