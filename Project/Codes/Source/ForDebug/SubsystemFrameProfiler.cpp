#include "ForDebug/SubsystemFrameProfiler.h"

#if defined(DEVELOPMENT)

#include <chrono>
#include <cstring>

namespace Atrum::Debug {

namespace {

constexpr std::size_t kIdCount =
    static_cast<std::size_t>(SubsystemFrameId::Count);

float lastFrameMs_[kIdCount]{};

std::int64_t NowTicks() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

} // namespace

const char *SubsystemFrameIdLabel(const SubsystemFrameId id) {
  switch (id) {
  case SubsystemFrameId::ProcessPump:
    return "Process (SDL/DT)";
  case SubsystemFrameId::ImGuiPrep:
    return "ImGui 準備";
  case SubsystemFrameId::Input:
    return "Input";
  case SubsystemFrameId::SceneUpdate:
    return "Scene Update";
  case SubsystemFrameId::SceneTransition:
    return "Scene 遷移";
  case SubsystemFrameId::Audio:
    return "Audio";
  case SubsystemFrameId::CameraPrepare:
    return "Camera 準備";
  case SubsystemFrameId::GfxPreDraw:
    return "Gfx PreDraw";
  case SubsystemFrameId::GfxSceneDraw:
    return "Gfx SceneDraw";
  case SubsystemFrameId::GfxPostDraw:
    return "Gfx PostDraw";
  case SubsystemFrameId::Count:
  default:
    return "?";
  }
}

void SubsystemFrameProfilerBeginFrame() { std::memset(lastFrameMs_, 0, sizeof(lastFrameMs_)); }

SubsystemFrameScope::SubsystemFrameScope(const SubsystemFrameId id)
    : id_(id), startTicks_(NowTicks()) {}

SubsystemFrameScope::~SubsystemFrameScope() {
  const std::int64_t endTicks = NowTicks();
  const float ms = static_cast<float>(endTicks - startTicks_) / 1'000'000.0f;
  const std::size_t index = static_cast<std::size_t>(id_);
  if (index < kIdCount) {
    lastFrameMs_[index] += ms;
  }
}

float SubsystemFrameLastSampleMs(const SubsystemFrameId id) {
  const std::size_t index = static_cast<std::size_t>(id);
  if (index >= kIdCount) {
    return 0.0f;
  }
  return lastFrameMs_[index];
}

} // namespace Atrum::Debug

#endif

namespace Atrum::Debug {

#if !defined(DEVELOPMENT)

const char *SubsystemFrameIdLabel(const SubsystemFrameId id) {
  (void)id;
  return "";
}

#endif

} // namespace Atrum::Debug