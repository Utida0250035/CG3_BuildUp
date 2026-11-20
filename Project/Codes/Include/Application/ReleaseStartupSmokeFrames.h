#pragma once

namespace Atrum::Application {

/// CI 用。`ATRUM_RELEASE_SMOKE_FRAMES` が正の整数ならそのフレーム数で正常終了する。未設定なら -1。
int ReadReleaseStartupSmokeFrameBudget();

} // namespace Atrum::Application