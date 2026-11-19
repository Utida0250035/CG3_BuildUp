#pragma once

namespace Atrum {

class Scene;

/// ムービーシーンのオーディオ方針（コード上の正本）
struct SceneMovieAudioPolicy final {
  /// Enter でループ BGM を開始しない（遷移元 Exit で停止済み）
  static constexpr bool kStartsLoopBgmOnEnter = false;
  /// トラック JSON 内の音声トラックは未接続（将来 MovieCameraTrackLoader 拡張）
  static constexpr bool kTrackJsonCarriesAudio = false;
  /// 復帰先シーンの EnterScene / フェード完了後 BGM が担当
  static constexpr bool kReturnSceneOwnsBgmRestart = true;

  /// EnterScene 直後: ムービー専用 BGM は開始しない
  static void OnEnterScene();

  /// Update: ムービー中の BGM スロット維持（現状は無操作）
  static void TickDuringMovie(bool hasPendingSceneChange);

  /// 復帰フェード開始前: returnScene 向けの遅延 BGM を登録
  static void PrepareReturnSceneBgmBeforeFade(Scene *returnScene);

  /// ExitScene: ムービーローカルのワンショット停止（将来拡張用）
  static void OnExitScene();
};

} // namespace Atrum