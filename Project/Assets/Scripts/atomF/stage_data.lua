-- atomF ステージ 1 のウェーブ定義（データのみ）。
-- 敵の種類名は Assets/atomF/parameters.json の enemy.variants。
-- 全種 Hp1。ウェーブごとに「動きの違う雑魚」を組み合わせて位置取りと連鎖爆発を考えさせる。
return {
  -- 湧く円周の半径をアリーナ内径に対する割合で指定
  spawnRadiusRatio = 0.78,
  -- 自機がこの距離以内の湧き位置は、反対側へずらす（湧いた瞬間の被弾を避ける）
  playerSafeRadius = 6.0,
  -- 1 体ずつ湧かせる間隔（秒）
  spawnIntervalSeconds = 0.22,
  -- 敵が居なくなってから次へ進むまでの間（秒）
  clearIntervalSeconds = 1.6,
  -- 残敵がこの時間（秒）倒されなくても、少数なら次へ進める（詰み防止）
  waveTimeoutSeconds = 45.0,
  waveTimeoutRemaining = 2,

  waves = {
    { spawns = { { "walker", 4 } } },
    { spawns = { { "walker", 3 }, { "runner", 3 } } },
    { spawns = { { "weaver", 3 }, { "charger", 2 }, { "shooter", 1 } } },
    { spawns = { { "orbiter", 3 }, { "shooter", 2 }, { "runner", 2 } } },
  },

  bossWarningSeconds = 3.0,
  bossIntroSeconds = 2.4, -- C++ 側 boss.appearSeconds と合わせる（HUD の BOSS 表示を出す目安）
  endDelaySeconds = 1.2,  -- 決着の瞬間を見せてから暗転する
}
