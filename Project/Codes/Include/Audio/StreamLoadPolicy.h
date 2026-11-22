#pragma once

#include <cstddef>
#include <string>

namespace Atrum::Audio {

inline constexpr size_t kDefaultStreamingLoadThresholdBytes = 5 * 1024 * 1024;

namespace StreamLoadPolicy {

size_t ThresholdBytes();
void SetThresholdBytes(size_t bytes);

} // namespace StreamLoadPolicy

// オンディスク容量に加え、拡張子から推定したデコード後 PCM サイズでも判定する。
bool IsSuitableStreamingLoad(const std::string& filePath, size_t fileSizeOnDisk = 0);

} // namespace Atrum::Audio