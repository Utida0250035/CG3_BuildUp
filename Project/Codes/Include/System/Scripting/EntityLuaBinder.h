#pragma once

#include "System/Scripting/ILuaBinder.h"

namespace Atrum {
class Entity;
}

namespace Atrum::Script {

/// エンティティ関連の機能を Lua へ公開する（詳細は Assets/Scripts/_api/entity.d.lua）。
///   Vec3 / Quat            … 値型（x,y,z[,w]）。Vec3 は + - * と length / normalized を持つ
///   Entities.*             … 検索（find / findAll / findByTag / findByRole / all / count）、
///                              生成（spawn(prefabPath, opts)）、破棄（destroy）、
///                              コンポーネント型名一覧（componentTypes）、タグ一覧（tagCatalog）
///   Entity（ハンドル）     … isValid / name 照合 / state / active / 位置・回転・スケール /
///                              親子 / タグ / コンポーネント有無 / 体力 など
class EntityLuaBinder final : public ILuaBinder {
public:
  void Register(sol::state &lua) override;
};

/// Lua 側 "Entity" usertype の実体（生ポインタ）。有効性は参照のたびにシーン走査で確認する。
/// onResolve / onUpdate の self 引数としてもこの型を渡す。
struct LuaEntityHandle {
  Atrum::Entity *ptr = nullptr;
};

/// スクリプトフックへ渡す self を作る。
/// onResolve 時点ではエンティティがシーン所有リストへ入っていないことがあるため、
/// mayBeUnadopted=true で「採用待ち」として有効扱いにする（採用後はシーン走査で確認される）。
LuaEntityHandle MakeEntityHandle(Atrum::Entity *entity, bool mayBeUnadopted);

} // namespace Atrum::Script
