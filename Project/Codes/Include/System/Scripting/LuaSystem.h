#pragma once
#include <memory>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <luajit.h>
}

// sol.hpp 内の u8"\xF0..." リテラルが C5321 を出し、TreatWarningAsError でビルドが止まるため外部ヘッダに限り抑止
#pragma warning(push)
#pragma warning(disable : 5321)
#include <sol/sol.hpp>
#pragma warning(pop)
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
  // ILuaBinder が不完全型のため、コンストラクタ／デストラクタの定義は
  // ILuaBinder.h を含む .cpp 側に置く（MSVC はコンストラクタでもメンバ破棄を実体化する）
  LuaSystem();
  ~LuaSystem();

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