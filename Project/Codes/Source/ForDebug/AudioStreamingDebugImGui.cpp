#include "ForDebug/AudioStreamingDebugImGui.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Audio/Audio.h"
#include "Audio/VoiceState.h"
#include "ForDebug/ImGui.h"

#include <cstdio>

namespace Atrum::Debug {

namespace {

bool &StreamingMemoryDebugShowFlag() {
  static bool showWindow = false;
  return showWindow;
}

const char *VoiceStateLabel(const Audio::VoiceState state) {
  switch (state) {
  case Audio::VoiceState::Playing:
    return "Playing";
  case Audio::VoiceState::Paused:
    return "Paused";
  case Audio::VoiceState::Waiting:
    return "Waiting";
  case Audio::VoiceState::Loading:
    return "Loading";
  case Audio::VoiceState::Streaming:
    return "Streaming";
  case Audio::VoiceState::Stopped:
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

  if (!ImGui::Begin("ストリーミング音声メモリ", &showWindow)) {
    ImGui::End();
    return;
  }

  Audio::StreamingMemoryDebugSnapshot snapshot{};
  Audio::Manager::Instance()->CollectStreamingMemoryDebug(snapshot);

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

  if (ImGui::BeginTable("voices", 6,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn("Idx");
    ImGui::TableSetupColumn("State");
    ImGui::TableSetupColumn("Sound");
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
      FormatBytes(row.streamingBufferBytes, buf, sizeof(buf));
      ImGui::TextUnformatted(buf);
      ImGui::TableSetColumnIndex(4);
      FormatBytes(row.remainingBytes, buf, sizeof(buf));
      ImGui::TextUnformatted(buf);
      ImGui::TableSetColumnIndex(5);
      ImGui::Text(row.isLoop ? "Y" : "-");
    }

    ImGui::EndTable();
  }

  ImGui::End();
}

} // namespace Atrum::Debug

#endif
