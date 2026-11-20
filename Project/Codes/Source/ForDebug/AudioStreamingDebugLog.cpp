#include "ForDebug/AudioStreamingDebugLog.h"

#if defined(DEVELOPMENT)

#include "Audio/VoiceState.h"
#include "ForDebug/Log.h"

#include <cstdio>
#include <string>

namespace Atrum::Debug {

namespace {

const char *VoiceStateLabel(const VoiceState state) {
  switch (state) {
  case VoiceState::Playing:
    return "Playing";
  case VoiceState::Paused:
    return "Paused";
  case VoiceState::Waiting:
    return "Waiting";
  case VoiceState::Loading:
    return "Loading";
  case VoiceState::Streaming:
    return "Streaming";
  case VoiceState::Stopped:
  default:
    return "Stopped";
  }
}

void LogLine(const std::string &line) { LogFile::Instance()->Log(line); }

} // namespace

void AppendStreamingMemoryDebugSnapshotToLog(
    const Audio::StreamingMemoryDebugSnapshot &snapshot) {
  LogLine("[StreamingDebug] snapshot begin");
  LogLine("[StreamingDebug] loadedShort bytes=" +
          std::to_string(snapshot.loadedShortSoundBytes) + " count=" +
          std::to_string(snapshot.loadedShortSoundCount));
  LogLine("[StreamingDebug] streaming bufferTotal=" +
          std::to_string(snapshot.streamingBufferBytesTotal) +
          " remainingTotal=" +
          std::to_string(snapshot.streamingRemainingBytesTotal) +
          " activeVoices=" +
          std::to_string(snapshot.activeStreamingVoiceCount) + "/" +
          std::to_string(snapshot.streamingVoicesCapacity));

  for (const Audio::StreamingVoiceMemoryRow &row : snapshot.voices) {
    char voiceLine[512];
    if (row.hasPlaybackTime) {
      std::snprintf(voiceLine, sizeof(voiceLine),
                    "[StreamingDebug] voice idx=%zu state=%s sound=%zu "
                    "playback=%.2fs queue=%u backlog=%.3f buf=%zu remain=%zu "
                    "loop=%d stream=%d",
                    row.voiceIndex, VoiceStateLabel(row.state), row.soundIndex,
                    row.playbackSeconds, row.buffersQueued,
                    row.decoderBacklogRatio, row.streamingBufferBytes,
                    row.remainingBytes, row.isLoop ? 1 : 0,
                    row.isStreamingFlag ? 1 : 0);
    } else {
      std::snprintf(voiceLine, sizeof(voiceLine),
                    "[StreamingDebug] voice idx=%zu state=%s sound=%zu "
                    "playback=- queue=%u backlog=%.3f buf=%zu remain=%zu "
                    "loop=%d stream=%d",
                    row.voiceIndex, VoiceStateLabel(row.state), row.soundIndex,
                    row.buffersQueued, row.decoderBacklogRatio,
                    row.streamingBufferBytes, row.remainingBytes,
                    row.isLoop ? 1 : 0, row.isStreamingFlag ? 1 : 0);
    }
    LogLine(voiceLine);
  }

  LogLine("[StreamingDebug] snapshot end");
}

} // namespace Atrum::Debug

#endif