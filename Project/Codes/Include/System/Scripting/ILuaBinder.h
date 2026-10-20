#pragma once

#include <sol/forward.hpp>

namespace Atrum::Script {

// Luaへのバインド用インターフェース
class ILuaBinder {
public:
  virtual ~ILuaBinder() = default;
  virtual void Register(sol::state &lua) = 0;
};

/*
バインド処理を派生クラスで記述
↓
派生クラスのインスタンスをLuaSystem::AddBinder<typename>()で生成・登録
↓
LuaSystem::RegisterBinds()で一括バインドを行わせる

※LuaSystemはシングルトンではなくEngineクラス内などで個別生成の方式
*/

} // namespace Atrum::Script