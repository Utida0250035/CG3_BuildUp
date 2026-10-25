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
}

/// BGM をループ再生（候補パスのうち存在するファイルを使用）
inline Audio::Handle PlayBgmLoop(
    const std::initializer_list<const char *> candidatePaths) {
  const std::string path = PlaySeDetail::ResolveExistingSePath(candidatePaths);
  if (path.empty()) {
    return {};
  }

  // 前シーンの BGM がボイスを掴んだまま残ると、以降 Play が失敗して無音になる
  Audio::Manager::Instance()->StopAllStreamingVoices();

  return PlaySeLoop(path);
}

} // namespace Atrum
