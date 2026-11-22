#include "Application/ReleaseStartupSmokeFrames.h"
#include "Application/ReleaseStartupSmokeCliParse.h"

namespace Atrum::Application {

int ResolveReleaseStartupSmokeFrameBudget(const char *commandLine) {
  const int fromCli = ReadSmokeFrameBudgetFromCommandLine(commandLine);
  if (fromCli > 0) {
    return fromCli;
  }
  return ReadReleaseStartupSmokeFrameBudget();
}

} // namespace Atrum::Application