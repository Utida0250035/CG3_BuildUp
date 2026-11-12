#include "ForDebug/AudioStreamingDebugImGui.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Audio/Audio.h"
#include "Engine/EngineAccess.h"
#include "Audio/VoiceState.h"
#include "ForDebug/ImGui.h"

#include <cstdio>

namespace Atrum::Debug {

namespace {

bool &StreamingMemoryDebugShowFlag() {
  static bool showWindow = false;
  return showWindow;
}

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

void FormatBytes(const size_t bytes, char *out, const size_t outSize) {
  if (bytes >= 1024 * 1024) {
    std::snprintf(out, outSize, "%.2f MiB", static_cast<double>(bytes) /
                                                 (1024.0 * 1024.0));
  } else if (bytes >= 1024) {
    std::snprintf(out, outSize, "%.1f KiB",
                  static_cast<double>(bytes) / 1024.0);
  } else {
    std::snprintf(out, outSize, "%zu B", bytes);
  }
}

} // namespace

bool IsStreamingMemoryDebugVisible() { return StreamingMemoryDebugShowFlag(); }

void SetStreamingMemoryDebugVisible(const bool visible) {
  StreamingMemoryDebugShowFlag() = visible;
}

void DrawAudioStreamingMemoryDebugWindow() {
  bool &showWindow = StreamingMemoryDebugShowFlag();
  if (!showWindow) {
    return;
  }

  if (!ImGui::Begin("ストリーミング音声デバッグ", &showWindow)) {
    ImGui::End();
    return;
  }

  Audio::StreamingMemoryDebugSnapshot snapshot{};
  ::Atrum::EngineAudioManager()->CollectStreamingMemoryDebug(snapshot);

  char buf[64];
  FormatBytes(snapshot.loadedShortSoundBytes, buf, sizeof(buf));
  ImGui::Text("短尺ロード済み: %s (%zu 件)", buf,
              snapshot.loadedShortSoundCount);

  const size_t streamingTotal = snapshot.streamingBufferBytesTotal +
                                snapshot.streamingRemainingBytesTotal;
  FormatBytes(streamingTotal, buf, sizeof(buf));
  ImGui::Text("ストリーミング占有: %s", buf);
  ImGui::Text("  バッファ: %zu B | remaining: %zu B",
              snapshot.streamingBufferBytesTotal,
              snapshot.streamingRemainingBytesTotal);
  ImGui::Text("稼働ボイス: %zu / %zu", snapshot.activeStreamingVoiceCount,
              snapshot.streamingVoicesCapacity);

  if (ImGui::BeginTable("voices", 9,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn("Idx");
    ImGui::TableSetupColumn("State");
    ImGui::TableSetupColumn("Sound");
    ImGui::TableSetupColumn("再生(s)");
    ImGui::TableSetupColumn("Queue");
    ImGui::TableSetupColumn("読込");
    ImGui::TableSetupColumn("Buffers");
    ImGui::TableSetupColumn("Remain");
    ImGui::TableSetupColumn("Loop");
    ImGui::TableHeadersRow();

    for (const Audio::StreamingVoiceMemoryRow &row : snapshot.voices) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%zu", row.voiceIndex);
      ImGui::TableSetColumnIndex(1);
      ImGui::TextUnformatted(VoiceStateLabel(row.state));
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%zu", row.soundIndex);
      ImGui::TableSetColumnIndex(3);
      if (row.hasPlaybackTime) {
        ImGui::Text("%.2f", row.playbackSeconds);
      } else {
        ImGui::TextUnformatted("-");
      }
      ImGui::TableSetColumnIndex(4);
      {
        char queueOverlay[32];
        std::snprintf(queueOverlay, sizeof(queueOverlay), "%u buf",
                      row.buffersQueued);
        ImGui::ProgressBar(row.queueFill01, ImVec2(-1.0f, 0.0f), queueOverlay);
      }
      ImGui::TableSetColumnIndex(5);
      ImGui::ProgressBar(row.decoderBacklog01, ImVec2(-1.0f, 0.0f),
                         row.isStreamingFlag ? "stream" : "idle");
      ImGui::TableSetColumnIndex(6);
      FormatBytes(row.streamingBufferBytes, buf, sizeof(buf));
      ImGui::TextUnformatted(buf);
      ImGui::TableSetColumnIndex(7);
      FormatBytes(row.remainingBytes, buf, sizeof(buf));
      ImGui::TextUnformatted(buf);
      ImGui::TableSetColumnIndex(8);
      ImGui::Text(row.isLoop ? "Y" : "-");
    }

    ImGui::EndTable();
  }

  ImGui::End();
}

} // namespace Atrum::Debug

#endif
