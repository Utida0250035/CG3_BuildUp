#pragma once

/// pendingDefeat 中はフル Maintain（PlayBgmLoop 再発火）を行わず、
/// 演出 Update 内の SyncSceneBgmDuringDefeatPresentation でストリームのみ維持する。
namespace Atrum {
namespace SceneGamePendingDefeatBgmPolicy {

constexpr bool kUseFullMaintainDuringDefeatPresentation = false;

} // namespace SceneGamePendingDefeatBgmPolicy
} // namespace Atrum