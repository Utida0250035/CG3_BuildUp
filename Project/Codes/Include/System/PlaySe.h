#pragma once

#include "Audio/Audio.h"
#include <filesystem>
#include <string>
#include <unordered_map>

namespace Atrum {

// ファイルが無い効果音は鳴らさない。同じパスは一度だけ読む
inline void PlaySe(const std::string &path) {
  if (!std::filesystem::exists(path)) {
    return;
  }

  static std::unordered_map<std::string, size_t> cache;
  Audio::AudioManager *audio = Audio::Manager::Instance();

  const auto found = cache.find(path);
  size_t index = 0;
  if (found == cache.end()) {
    index = audio->Load(path);
    cache.emplace(path, index);
  } else {
    index = found->second;
  }

  audio->Play(index);
}

} // namespace Atrum
