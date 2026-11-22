#pragma once

#if defined(DEVELOPMENT)

#include "Audio/AudioStreamingDebugStats.h"

namespace Atrum::Debug {

/// ストリーミング診断スナップショットを LogFile へ追記（共有・再現用）。
void AppendStreamingMemoryDebugSnapshotToLog(
    const Audio::StreamingMemoryDebugSnapshot &snapshot);

} // namespace Atrum::Debug

#endif