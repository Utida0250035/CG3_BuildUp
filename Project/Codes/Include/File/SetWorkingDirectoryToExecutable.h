#pragma once

namespace Atrum::File {

// 相対パス ./Assets を解決するため作業フォルダを設定する。
// DEVELOPMENT では Project/Assets を優先（パーティクル JSON 等の編集を即反映）。
bool SetWorkingDirectoryToExecutableDirectory();

} // namespace Atrum::File
