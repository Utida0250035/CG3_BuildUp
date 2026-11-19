#pragma once

#include <cstddef>
#include <string>

namespace Atrum::Audio {

// 次行「閾値 5MB ハードコード」で設定化予定。ここを単一参照点にする。
inline constexpr size_t kStreamingLoadThresholdBytes = 5 * 1024 * 1024;

// オンディスク容量に加え、拡張子から推定したデコード後 PCM サイズでも判定する。
bool IsSuitableStreamingLoad(const std::string& filePath, size_t fileSizeOnDisk = 0);

} // namespace Atrum::Audio