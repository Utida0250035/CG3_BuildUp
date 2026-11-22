#include "Application/ReleaseStartupSmokeFrames.h"

#include <cstdlib>

namespace Atrum::Application {

int ReadReleaseStartupSmokeFrameBudget() {
#if defined(_MSC_VER)
  char *raw = nullptr;
  size_t rawLength = 0;
  if (_dupenv_s(&raw, &rawLength, "ATRUM_RELEASE_SMOKE_FRAMES") != 0 ||
      raw == nullptr || raw[0] == '\0') {
    std::free(raw);
    return -1;
  }
  const int frames = std::atoi(raw);
  std::free(raw);
#else
  const char *raw = std::getenv("ATRUM_RELEASE_SMOKE_FRAMES");
  if (raw == nullptr || raw[0] == '\0') {
    return -1;
  }
  const int frames = std::atoi(raw);
#endif
  if (frames <= 0) {
    return -1;
  }
  return frames;
}

} // namespace Atrum::Application