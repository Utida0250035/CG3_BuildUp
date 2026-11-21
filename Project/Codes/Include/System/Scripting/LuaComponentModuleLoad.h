#pragma once

#include <sol/forward.hpp>
#include <string>

namespace Atrum::Script {

/// Lua スクリプトを読み込み、return されたテーブルを返す（失敗時は空テーブル）
sol::table LoadLuaComponentModule(const std::string &scriptPath,
                                  std::string *outError = nullptr);

std::string ResolveLuaScriptPathForComponent(const std::string &path);

} // namespace Atrum::Script