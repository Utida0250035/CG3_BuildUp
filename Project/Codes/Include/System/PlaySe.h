#pragma once

#include "Audio/Audio.h"
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
  Audio::AudioManager *audio = Audio::Manager::Instance();

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

  Audio::Manager::Instance()->Play(index);
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

  return Audio::Manager::Instance()->Play(index, true);
}

inline void StopSe(const Audio::Handle &handle) {
  // PlayShort 以前は isSuccess が未設定でも playId で停止できるようにする
  if (!handle.isSuccess && handle.playId == 0) {
    return;
  }

  Audio::Manager::Instance()->Stop(handle);
}

/// シーン BGM を停止（ハンドル不一致で残ったストリーミングボイスも解放）
inline void StopBgmLoop(const Audio::Handle &handle) {
  StopSe(handle);
  Audio::Manager::Instance()->StopAllStreamingVoices();
  Audio::Manager::Instance()->StopAllLoopingShortVoices();
}

inline void PauseBgmLoop(const Audio::Handle &handle) {
  if (!handle.isSuccess) {
    return;
  }
  Audio::Manager::Instance()->PauseSceneBgm(handle);
}

inline void ResumeBgmLoop(const Audio::Handle &handle) {
  if (!handle.isSuccess) {
    return;
  }
  Audio::Manager::Instance()->ResumeSceneBgm(handle);
}

inline bool IsBgmLoopPaused(const Audio::Handle &handle) {
  return Audio::Manager::Instance()->IsSceneBgmPaused(handle);
}

/// 失敗時は負値
inline float GetBgmLoopPlaybackSeconds(const Audio::Handle &handle) {
  float seconds = -1.0f;
  if (Audio::Manager::Instance()->GetSceneBgmPlaybackSeconds(handle,
                                                               seconds)) {
    return seconds;
  }
  return -1.0f;
}

inline bool SetBgmLoopPlaybackSeconds(const Audio::Handle &handle,
                                      float seconds) {
  return Audio::Manager::Instance()->SetSceneBgmPlaybackSeconds(handle,
                                                                seconds);
}

inline bool SetSeFrequencyRatio(const Audio::Handle &handle, float ratio) {
  return Audio::Manager::Instance()->SetHandleFrequencyRatio(handle, ratio);
}

inline bool GetSeFrequencyRatio(const Audio::Handle &handle, float &outRatio) {
  return Audio::Manager::Instance()->GetHandleFrequencyRatio(handle, outRatio);
}

inline bool SetSeVolume(const Audio::Handle &handle, float volume) {
  return Audio::Manager::Instance()->SetHandleVolume(handle, volume);
}

inline bool GetSeVolume(const Audio::Handle &handle, float &outVolume) {
  return Audio::Manager::Instance()->GetHandleVolume(handle, outVolume);
}

inline bool SetBgmLoopPitch(const Audio::Handle &handle, float pitchRatio) {
  return SetSeFrequencyRatio(handle, pitchRatio);
}

inline bool SetBgmLoopVolume(const Audio::Handle &handle, float volume) {
  return SetSeVolume(handle, volume);
}

/// BGM をループ再生（候補パスのうち存在するファイルを使用）
inline Audio::Handle PlayBgmLoop(
    const std::initializer_list<const char *> candidatePaths) {
  const std::string path = PlaySeDetail::ResolveExistingSePath(candidatePaths);
  if (path.empty()) {
    return {};
  }

  const size_t index = PlaySeDetail::GetOrLoadSeIndex(path);
  if (index == static_cast<size_t>(-1)) {
    return {};
  }

  Audio::AudioManager *audio = Audio::Manager::Instance();
  Audio::Handle handle = audio->PlaySceneBgm(index);
  if (!handle.isSuccess) {
    // デコーダ／ボイス状態の不整合時に 1 度だけ再試行
    handle = audio->PlaySceneBgm(index);
  }
  return handle;
}

} // namespace Atrum
