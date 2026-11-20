#pragma once

namespace Atrum::SceneTransitionBgm {

/// Play 向けシーン遷移 BGM 方針（瞬断回避・クロスフェードの第一歩）
///
/// 1. 遷移中（各シーン Update）: SyncSceneBgmForChangeSceneFade でダック＋一時停止
/// 2. Exit（フェード実行中）: ReleaseSceneBgmOnExitDuringFade → CommandChangeScene へ引き継ぎ
/// 3. フェードイン完了: OnSceneTransitionEffectFinished で旧 BGM 停止、次曲は deferred Enter
/// 4. 同一曲継続: SceneGame::resumeGameBgmPlaybackSeconds_（Game→Game 再戦）
///
/// 真の二重トラック・クロスフェードは将来 AudioManager で拡張する。

inline constexpr bool kUseFadeDuckInsteadOfInstantStop = true;

} // namespace Atrum::SceneTransitionBgm