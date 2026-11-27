#pragma once

#include "System/Scripting/ILuaBinder.h"

namespace Atrum::Script {

/// atomF のステージ進行を Lua で書くための API。敵・ボスの挙動そのものは C++ コンポーネント＋
/// parameters.json で定義し、「いつ何を出すか」（ウェーブ・ボス登場・HUD）を Lua 側に置く。
///   AtomF.spawnEnemy(variantName, x, z)  雑魚を出す（名前は parameters.json の enemy.variants）
///   AtomF.spawnBoss(x, z)                ボスを出す（居れば無視）
///   AtomF.variantNames()                 雑魚の種類名の配列
///   AtomF.arenaRadius()                  アリーナ内径（m）
///   AtomF.playerPosition()               自機の x, z（居なければ nil）
///   AtomF.bossHealth() / bossMaxHealth() 現在 Hp（居なければ -1）/ 最大 Hp
///   AtomF.bossState()                    "none" / "appear" / "fight" / "stunned" / "dying"
///   AtomF.bossPhase2()                   第 2 形態か
class AtomFLuaBinder final : public ILuaBinder {
public:
  void Register(sol::state &lua) override;
};

} // namespace Atrum::Script
