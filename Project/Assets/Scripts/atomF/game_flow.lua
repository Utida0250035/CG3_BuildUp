-- atomF 本編の進行（1 ステージ）:
--   ウェーブ 1 → ウェーブ 2 → ウェーブ 3 → ボス → リザルト。
-- 自機の Hp が 0 になったらいつでもリザルトへ遷移する。
-- 雑魚の種類（walker/runner/weaver/charger）は parameters.json の enemy.variants、
-- ボスの数値は boss を参照する。ここは「いつ・どこに・何を出すか」だけを書く。
local Paths = require("atomF.scene_paths")

local M = {}

local TWO_PI = 6.28318530718

local END_DELAY_SECONDS      = 1.0 -- 自機撃墜の瞬間を見せてから暗転する
local CLEAR_DELAY_SECONDS    = 2.0 -- ボス撃破の余韻
local FIRST_WAVE_DELAY       = 1.2
local WAVE_INTERVAL_SECONDS  = 2.0
local BOSS_WARNING_SECONDS   = 3.0 -- 最後のウェーブ全滅からボス出現まで
local BOSS_SPAWN_X, BOSS_SPAWN_Z = 0.0, 8.0

-- 円周上に names を順番に割り当てて並べる。
local function ring(names, count, radius, angleOffset)
  local spawns = {}
  for i = 0, count - 1 do
    local angle = angleOffset + TWO_PI * i / count
    spawns[#spawns + 1] = {
      name = names[(i % #names) + 1],
      x = math.cos(angle) * radius,
      z = math.sin(angle) * radius,
    }
  end
  return spawns
end

local function concat(a, b)
  for _, v in ipairs(b) do
    a[#a + 1] = v
  end
  return a
end

-- ウェーブ定義。各ウェーブの敵が全滅したら次へ進む。
local WAVES = {
  -- 1: 基本の歩兵（連鎖爆発の練習）
  ring({ "walker", "walker", "runner" }, 6, 10.0, 0.0),
  -- 2: 蛇行・突進が混ざる
  ring({ "weaver", "charger", "walker", "runner" }, 8, 11.0, 0.3),
  -- 3: 二重の輪。内側は遅い突進型、外側は速い足
  concat(ring({ "charger", "weaver" }, 6, 7.0, 0.0),
         ring({ "runner", "walker", "runner" }, 9, 12.5, 0.2)),
}

local STATE_WAIT_SPAWN = "waitSpawn"   -- ウェーブ出現まで待つ
local STATE_WAVE       = "wave"        -- 出現直後〜全滅待ち
local STATE_BOSS_WAIT  = "bossWait"    -- ボス出現までの間
local STATE_BOSS       = "boss"        -- ボス戦
local STATE_CLEAR      = "clear"       -- 撃破の余韻
local STATE_DEAD       = "dead"        -- 自機撃墜の余韻

local state = STATE_WAIT_SPAWN
local stateElapsed = 0.0
local waveIndex = 1
local waveSeenEnemy = false
local bossSeen = false
local seenPlayer = false

local function setState(next)
  state = next
  stateElapsed = 0.0
end

local function spawnWave(index)
  for _, s in ipairs(WAVES[index]) do
    SceneFlow.spawnEnemy(s.name, s.x, s.z)
  end
  waveSeenEnemy = false
  setState(STATE_WAVE)
end

function M.onUpdate(dt)
  if SceneFlow.isChanging() then
    return
  end

  stateElapsed = stateElapsed + dt

  -- 自機撃墜はどの段階でも優先する。
  local playerHealth = SceneFlow.healthByRole("player") -- 自機が居なければ -1
  if playerHealth >= 0 then
    seenPlayer = true
  end
  local playerDead = playerHealth == 0 or (seenPlayer and playerHealth < 0)
  if playerDead and state ~= STATE_DEAD and state ~= STATE_CLEAR then
    setState(STATE_DEAD)
  end

  if state == STATE_DEAD then
    if stateElapsed >= END_DELAY_SECONDS then
      SceneFlow.change(Paths.RESULT)
    end
    return
  end

  if state == STATE_CLEAR then
    if stateElapsed >= CLEAR_DELAY_SECONDS then
      SceneFlow.change(Paths.RESULT)
    end
    return
  end

  if state == STATE_WAIT_SPAWN then
    local delay = (waveIndex == 1) and FIRST_WAVE_DELAY or WAVE_INTERVAL_SECONDS
    if stateElapsed >= delay then
      spawnWave(waveIndex)
    end
    return
  end

  local enemyCount = SceneFlow.countByRole("enemy")

  if state == STATE_WAVE then
    -- spawnEnemy で積んだ敵は次フレームからシーンに入るので、見えるまで待つ。
    if enemyCount > 0 then
      waveSeenEnemy = true
    end
    if waveSeenEnemy and enemyCount == 0 then
      if waveIndex < #WAVES then
        waveIndex = waveIndex + 1
        setState(STATE_WAIT_SPAWN)
      else
        setState(STATE_BOSS_WAIT)
      end
    end
    return
  end

  if state == STATE_BOSS_WAIT then
    if stateElapsed >= BOSS_WARNING_SECONDS then
      SceneFlow.spawnBoss(BOSS_SPAWN_X, BOSS_SPAWN_Z)
      bossSeen = false
      setState(STATE_BOSS)
    end
    return
  end

  if state == STATE_BOSS then
    local bossHealth = SceneFlow.bossHealth() -- ボスが居なければ -1
    if bossHealth >= 0 then
      bossSeen = true
    end
    if bossSeen and bossHealth < 0 then
      setState(STATE_CLEAR)
    end
  end
end

return M
