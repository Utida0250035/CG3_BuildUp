#pragma once

#include "Audio/Audio.h"
#include "Engine/AtrumEngine.h"
#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace Atrum {

namespace PlaySeDetail {

inline size_t GetOrLoadSeIndex(const std::string &path) {
  if (!std::filesystem::exists(path)) {
    return static_cast<size_t>(-1);
  }

  static std::unordered_map<std::string, size_t> cache;
  Audio::AudioManager *audio = ::Atrum::EngineAudioManager();

  const auto found = cache.find(path);
  if (found == cache.end()) {
    const size_t index = audio->Load(path);
    if (index == static_cast<size_t>(-1)) {
      return static_cast<size_t>(-1);
    }
    cache.emplace(path, index);
    return index;
  }

  return found->second;
}

inline std::string ResolveExistingSePath(
    const std::initializer_list<const char *> candidatePaths) {
  for (const char *path : candidatePaths) {
    if (path != nullptr && std::filesystem::exists(path)) {
      return path;
    }
  }
  return {};
}

} // namespace PlaySeDetail

// ファイルが無い効果音は鳴らさない。同じパスは一度だけ読む
inline void PlaySe(const std::string &path) {
  const size_t index = PlaySeDetail::GetOrLoadSeIndex(path);
  if (index == static_cast<size_t>(-1)) {
    return;
  }

  ::Atrum::EngineAudioManager()->Play(index);
}

/// Assets / Resources など複数候補から存在する SE を再生
inline void PlaySeCandidates(
    const std::initializer_list<const char *> candidatePaths) {
  const std::string path =
      PlaySeDetail::ResolveExistingSePath(candidatePaths);
  if (path.empty()) {
    return;
  }

  PlaySe(path);
}

/// 決定・スキップ（Submit）用 SE
inline void PlaySubmitSe() {
  PlaySeCandidates({"./Assets/Audios/seSubmit.mp3",
                    "./Resources/seSubmit.mp3"});
}

/// シーン遷移フェードイン開始時の SE（専用ファイルが無ければ submit へフォールバック）
inline void PlaySceneTransitionSe() {
  PlaySeCandidates({"./Assets/Audios/seSceneTransition.mp3",
                    "./Resources/seSceneTransition.mp3",
                    "./Assets/Audios/seSubmit.mp3",
                    "./Resources/seSubmit.mp3"});
}

/// 自機ジャンプ
inline void PlayPlayerJumpSe() {
  PlaySeCandidates({"./Assets/Audios/sePlayerJump.mp3",
                    "./Resources/sePlayerJump.mp3"});
}

/// ボス／敵の弾発射
inline void PlayEnemyShotSe() {
  PlaySeCandidates({"./Assets/Audios/seEnemyShot.mp3",
                    "./Resources/seEnemyShot.mp3"});
}

/// ループ再生。ファイルが無い場合は失敗した Handle を返す
inline Audio::Handle PlaySeLoop(const std::string &path) {
  const size_t index = PlaySeDetail::GetOrLoadSeIndex(path);
  if (index == static_cast<size_t>(-1)) {
    return {};
  }

  return ::Atrum::EngineAudioManager()->Play(index, true);
}

inline void StopSe(const Audio::Handle &handle) {
  // PlayShort 以前は isSuccess が未設定でも playId で停止できるようにする
  if (!handle.isSuccess && handle.playId == 0) {
    return;
  }

  ::Atrum::EngineAudioManager()->Stop(handle);
}

/// シーン BGM を停止（汎用ストリーミング SE は巻き添えしない）
inline void StopBgmLoop(const Audio::Handle &handle) {
  StopSe(handle);

  if (!handle.isSuccess && handle.playId == 0) {
    ::Atrum::EngineAudioManager()->StopDedicatedSceneBgmVoice();
  }
}

inline void PauseBgmLoop(const Audio::Handle &handle) {
  if (!handle.isSuccess) {
    return;
  }
  ::Atrum::EngineAudioManager()->PauseSceneBgm(handle);
}

inline void ResumeBgmLoop(const Audio::Handle &handle) {
  if (!handle.isSuccess) {
    return;
  }
  ::Atrum::EngineAudioManager()->ResumeSceneBgm(handle);
}

inline bool IsBgmLoopPaused(const Audio::Handle &handle) {
  return ::Atrum::EngineAudioManager()->IsSceneBgmPaused(handle);
}

/// 失敗時は負値
inline float GetBgmLoopPlaybackSeconds(const Audio::Handle &handle) {
  float seconds = -1.0f;
  if (::Atrum::EngineAudioManager()->GetSceneBgmPlaybackSeconds(handle,
                                                               seconds)) {
    return seconds;
  }
  return -1.0f;
}

inline bool SetBgmLoopPlaybackSeconds(const Audio::Handle &handle,
                                      float seconds) {
  return ::Atrum::EngineAudioManager()->SetSceneBgmPlaybackSeconds(handle,
                                                                seconds);
}

inline bool SetSeFrequencyRatio(const Audio::Handle &handle, float ratio) {
  return ::Atrum::EngineAudioManager()->SetHandleFrequencyRatio(handle, ratio);
}

inline bool GetSeFrequencyRatio(const Audio::Handle &handle, float &outRatio) {
  return ::Atrum::EngineAudioManager()->GetHandleFrequencyRatio(handle, outRatio);
}

inline bool SetSeVolume(const Audio::Handle &handle, float volume) {
  return ::Atrum::EngineAudioManager()->SetHandleVolume(handle, volume);
}

inline bool GetSeVolume(const Audio::Handle &handle, float &outVolume) {
  return ::Atrum::EngineAudioManager()->GetHandleVolume(handle, outVolume);
}

inline bool SetBgmLoopPitch(const Audio::Handle &handle, float pitchRatio) {
  return SetSeFrequencyRatio(handle, pitchRatio);
}

inline bool SetBgmLoopVolume(const Audio::Handle &handle, float volume) {
  return SetSeVolume(handle, volume);
}

inline Audio::Handle PlayBgmLoopCore(
    const std::initializer_list<const char *> candidatePaths) {
  const std::string path = PlaySeDetail::ResolveExistingSePath(candidatePaths);
  if (path.empty()) {
    return {};
  }

  const size_t index = PlaySeDetail::GetOrLoadSeIndex(path);
  if (index == static_cast<size_t>(-1)) {
    return {};
  }

  Audio::AudioManager *audio = ::Atrum::EngineAudioManager();
  Audio::Handle handle = audio->PlaySceneBgm(index);
  if (!handle.isSuccess) {
    // デコーダ／ボイス状態の不整合時に 1 度だけ再試行
    handle = audio->PlaySceneBgm(index);
  }
  return handle;
}

/// フェードイン完了後（CommandChangeScene から）のみ呼ぶ
inline Audio::Handle PlayBgmLoopAfterSceneFade(
    const std::initializer_list<const char *> candidatePaths) {
  return PlayBgmLoopCore(candidatePaths);
}

/// BGM をループ再生（候補パスのうち存在するファイルを使用）
inline Audio::Handle PlayBgmLoop(
    const std::initializer_list<const char *> candidatePaths);

inline void AssignPendingFadeSyncedSceneBgm(Audio::Handle &slot);

} // namespace Atrum

#include "Scene/CommandChangeScene.h"

namespace Atrum {

/// BGM をループ再生（候補パスのうち存在するファイルを使用）
inline Audio::Handle PlayBgmLoop(
    const std::initializer_list<const char *> candidatePaths) {
  if (CommandChangeScene::Instance()->ShouldDeferEnterSceneBgm()) {
    CommandChangeScene::Instance()->RegisterDeferredEnterSceneBgm(
        candidatePaths);
    return {};
  }

  return PlayBgmLoopCore(candidatePaths);
}

inline void AssignPendingFadeSyncedSceneBgm(Audio::Handle &slot) {
  if (!slot.isSuccess) {
    slot = CommandChangeScene::Instance()->TryTakePendingEnterSceneBgmHandle();
  }
}

/// ChangeSceneFade 実行中は BGM をダック＋一時停止し、通常時はフル音量へ戻す
inline void SyncSceneBgmForChangeSceneFade(Audio::Handle &handle) {
  if (!handle.isSuccess) {
    return;
  }

  constexpr float kFadeDuckVolume = 0.12f;

  if (CommandChangeScene::Instance()->GetIsExecute()) {
    SetBgmLoopVolume(handle, kFadeDuckVolume);
    if (!IsBgmLoopPaused(handle)) {
      PauseBgmLoop(handle);
    }
    return;
  }

  if (IsBgmLoopPaused(handle)) {
    ResumeBgmLoop(handle);
  }
  SetBgmLoopVolume(handle, 1.0f);
}

} // namespace Atrum
