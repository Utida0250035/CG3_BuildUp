#pragma once
#include <memory>
#include <sol/sol.hpp>
#include <string>
#include <vector>

namespace Atrum::Script {

class ILuaBinder;

// Luaスクリプト環境クラス
class LuaSystem {
private:
  // Luaの仮想マシン
  sol::state lua_{};
  // バインド処理の配列
  std::vector<std::unique_ptr<ILuaBinder>> binders_{};

public:
  LuaSystem() = default;
  ~LuaSystem() = default;

  // solなどの初期化
  bool Initialize();

  // バインド処理を登録
  template <typename T, typename... Args> void AddBinder(Args &&...args) {
    binders_.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
  }

  // バインド処理を一括実行
  void RegisterBinds();
  // .luaファイルの実行
  bool ExecuteFile(const std::string &filePath);
  // Luaスクリプト文字列の実行
  bool ExecuteStr(const std::string &code);
  // Luaの仮想マシンの取得
  sol::state &GetState();
};

} // namespace Atrum::Script