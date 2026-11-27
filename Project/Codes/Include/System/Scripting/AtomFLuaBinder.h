#pragma once

#include "System/Scripting/ILuaBinder.h"

namespace Atrum::Script {

/// atomF のステージ進行を Lua（Assets/Scripts/atomF/stage.lua）で書くための API。
///   AtomF.variantCount() / variantIndex(name)   … 雑魚の種類（parameters の enemy.variants）
///   AtomF.spawnEnemy(variant, x, z)             … 雑魚を出す（variant は添字 0 起点 or 名前）
///   AtomF.spawnBoss(x, z)                       … ボスを出す（出現演出つき）
///   AtomF.bossHealth() / bossMaxHealth()        … ボス不在なら -1
///   AtomF.bossState()                           … "appearing"|"walking"|"telegraph"|... 不在は ""
///   AtomF.playerPosition()                      … 自機の x, z（不在なら 0, 0）
///   AtomF.arenaRadius()                         … 円形フィールドの内半径(m)
class AtomFLuaBinder final : public ILuaBinder {
public:
  void Register(sol::state &lua) override;
};

} // namespace Atrum::Script
