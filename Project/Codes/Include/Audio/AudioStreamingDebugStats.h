#pragma once

#if defined(DEVELOPMENT)

#include "Audio/VoiceState.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Atrum::Audio {

struct StreamingVoiceMemoryRow {
  size_t voiceIndex = 0;
  VoiceState state = VoiceState::Stopped;
  size_t soundIndex = 0;
  uint64_t playId = 0;
  size_t streamingBufferBytes = 0;
  size_t remainingBytes = 0;
  bool isLoop = false;
  bool isStreamingFlag = false;
  float playbackSeconds = 0.0f;
  bool hasPlaybackTime = false;
  uint32_t buffersQueued = 0;
  uint64_t samplesPlayed = 0;
  /// キュー充填（0〜1、BuffersQueued / kBufferCount）
  float queueFill01 = 0.0f;
  /// デコーダ残りバイト / 1チャンク（未クランプ。1超で remaining 肥大）
  float decoderBacklogRatio = 0.0f;
  /// 上記を 0〜1 にクランプ（プログレスバー用）
  float decoderBacklog01 = 0.0f;
};

struct StreamingMemoryDebugSnapshot {
  size_t loadedShortSoundBytes = 0;
  size_t loadedShortSoundCount = 0;
  size_t streamingVoicesCapacity = 0;
  size_t streamingBufferBytesTotal = 0;
  size_t streamingRemainingBytesTotal = 0;
  size_t activeStreamingVoiceCount = 0;
  std::vector<StreamingVoiceMemoryRow> voices;
};

} // namespace Atrum::Audio

#endif
