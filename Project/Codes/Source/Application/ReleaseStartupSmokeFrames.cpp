#include "Application/ReleaseStartupSmokeFrames.h"

#include <cstdlib>

namespace Atrum::Application {

int ReadReleaseStartupSmokeFrameBudget() {
  const char *raw = std::getenv("ATRUM_RELEASE_SMOKE_FRAMES");
  if (raw == nullptr || raw[0] == '\0') {
    return -1;
  }
  const int frames = std::atoi(raw);
  if (frames <= 0) {
    return -1;
  }
  return frames;
}

} // namespace Atrum::Application