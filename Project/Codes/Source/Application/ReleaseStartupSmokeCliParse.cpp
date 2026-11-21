#include "Application/ReleaseStartupSmokeCliParse.h"

#include <cstdlib>
#include <cstring>

namespace Atrum::Application {

namespace {

const char kSmokeFramesFlag[] = "--smoke-frames";

int ParsePositiveFrames(const char *text) {
  if (text == nullptr || text[0] == '\0') {
    return -1;
  }
  char *end = nullptr;
  const long value = std::strtol(text, &end, 10);
  if (end == text || value <= 0 || value > 1'000'000) {
    return -1;
  }
  return static_cast<int>(value);
}

const char *SkipWhitespace(const char *text) {
  while (text != nullptr && (*text == ' ' || *text == '\t')) {
    ++text;
  }
  return text;
}

} // namespace

int ReadSmokeFrameBudgetFromCommandLine(const char *commandLine) {
  if (commandLine == nullptr || commandLine[0] == '\0') {
    return -1;
  }
  const char *cursor = commandLine;
  while ((cursor = std::strstr(cursor, kSmokeFramesFlag)) != nullptr) {
    cursor += sizeof(kSmokeFramesFlag) - 1;
    cursor = SkipWhitespace(cursor);
    if (cursor[0] == '=') {
      ++cursor;
      cursor = SkipWhitespace(cursor);
      const int frames = ParsePositiveFrames(cursor);
      if (frames > 0) {
        return frames;
      }
    } else if (cursor[0] != '\0' && cursor[0] != '-') {
      const int frames = ParsePositiveFrames(cursor);
      if (frames > 0) {
        return frames;
      }
    }
    ++cursor;
  }
  return -1;
}

} // namespace Atrum::Application