#pragma once

namespace Atrum::Application {

/// WinMain の lpCmdLine から `--smoke-frames=<N>` / `--smoke-frames <N>` を読む。無ければ -1。
int ReadSmokeFrameBudgetFromCommandLine(const char *commandLine);

} // namespace Atrum::Application