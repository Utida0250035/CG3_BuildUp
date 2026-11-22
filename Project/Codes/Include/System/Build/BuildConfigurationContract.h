#pragma once

namespace Atrum::Build {

/// 起動ログ・診断用の人間可読ラベル（必ずいずれか 1 構成に一致）。
const char *ConfigurationLabel();

/// Release|x64 のみ true（出荷候補）。
bool IsShipCandidateBuild();

} // namespace Atrum::Build