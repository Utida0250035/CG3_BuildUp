#pragma once

namespace Atrum::Application {

/// CI 用。`ATRUM_RELEASE_SMOKE_FRAMES` が正の整数ならそのフレーム数で正常終了する。未設定なら -1。
int ReadReleaseStartupSmokeFrameBudget();

/// CLI `--smoke-frames` を優先し、無ければ環境変数を参照。どちらも無ければ -1。
int ResolveReleaseStartupSmokeFrameBudget(const char *commandLine);

} // namespace Atrum::Application