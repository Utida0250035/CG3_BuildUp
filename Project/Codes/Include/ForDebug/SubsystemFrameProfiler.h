#pragma once

#include <cstdint>

namespace Atrum::Debug {

enum class SubsystemFrameId : uint8_t {
  ProcessPump = 0,
  ImGuiPrep,
  Input,
  SceneUpdate,
  SceneTransition,
  Audio,
  CameraPrepare,
  GfxPreDraw,
  GfxSceneDraw,
  GfxPostDraw,
  Count
};

const char *SubsystemFrameIdLabel(const SubsystemFrameId id);

#if defined(DEVELOPMENT)

class SubsystemFrameScope {
  SubsystemFrameId id_;
  std::int64_t startTicks_;

public:
  explicit SubsystemFrameScope(const SubsystemFrameId id);
  ~SubsystemFrameScope();
};

void SubsystemFrameProfilerBeginFrame();
float SubsystemFrameLastSampleMs(const SubsystemFrameId id);

#if defined(USE_IMGUI)

void DrawSubsystemFrameProfilerDebug();
void SetSubsystemFrameProfilerDetailVisible(const bool visible);
bool IsSubsystemFrameProfilerDetailVisible();

#endif

#else

class SubsystemFrameScope {
public:
  explicit SubsystemFrameScope(const SubsystemFrameId /*id*/) {}
};

inline void SubsystemFrameProfilerBeginFrame() {}

#endif

} // namespace Atrum::Debug