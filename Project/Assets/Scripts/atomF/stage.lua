-- atomF ステージ進行（1 ステージ作り込み）: 雑魚ウェーブ → ボス戦。
-- 敵・ボスの出現は C++ の AtomF.spawnEnemy / AtomF.spawnBoss、数値は parameters.json。
-- 使い方: game_flow.lua が毎フレーム Stage.update(dt) を呼び、"cleared" が返ったら勝利。
local M = {}

-- 雑魚ウェーブ。{ 種類名, 体数 } の並び。全種 Hp1・耐久系能力なし（種類は parameters.json の enemy.variants）。
local WAVES = {
  { { "walker", 4 } },
  { { "walker", 2 }, { "runner", 2 } },
  { { "weaver", 3 }, { "charger", 2 } },
  { { "sprinter", 3 }, { "lurker", 2 }, { "walker", 2 } },
  { { "runner", 2 }, { "weaver", 2 }, { "charger", 2 }, { "lurker", 2 }, { "sprinter", 2 } },
}

local INTRO_SECONDS = 1.5          -- 開始直後の猶予
local WAVE_REST_SECONDS = 1.6      -- ウェーブ全滅から次の出現まで
local BOSS_INTRO_SECONDS = 2.5     -- 最終ウェーブ全滅からボス出現まで（静けさ）
local SPAWN_STAGGER_SECONDS = 0.18 -- 1 体ずつ時間差で出す
local SPAWN_RADIUS_RATIO = 0.72    -- フィールド半径に対する出現半径
local MIN_SPAWN_DISTANCE = 5.0     -- 自機からこれ未満には出さない
local AMMO_CHECK_SECONDS = 5.0     -- ボス戦で玉（雑魚）が尽きたとき補充するまで
local AMMO_COUNT = 2
local CLEAR_DELAY_SECONDS = 1.5    -- ボス消滅後、リザルトへ行くまでの余韻

local state, timer
local waveIndex, spawnQueue, spawnTimer
local bossSeen, ammoTimer, bossHpShown

function M.reset()
  state = "intro"
  timer = 0.0
  waveIndex = 0
  spawnQueue = {}
  spawnTimer = 0.0
  bossSeen = false
  ammoTimer = 0.0
  bossHpShown = nil
end

M.reset()

-- 自機から離れた円周上の点を選ぶ
local function pickSpawnPoint()
  local px, pz = AtomF.playerPosition()
  local radius = AtomF.arenaRadius() * SPAWN_RADIUS_RATIO
  local x, z = radius, 0.0
  for _ = 1, 10 do
    local angle = math.random() * math.pi * 2.0
    x, z = math.cos(angle) * radius, math.sin(angle) * radius
    local dx, dz = x - px, z - pz
    if dx * dx + dz * dz >= MIN_SPAWN_DISTANCE * MIN_SPAWN_DISTANCE then
      break
    end
  end
  return x, z
end

local function queueWave(index)
  spawnQueue = {}
  for _, entry in ipairs(WAVES[index]) do
    for _ = 1, entry[2] do
      spawnQueue[#spawnQueue + 1] = entry[1]
    end
  end
  spawnTimer = 0.0
end

local function flushSpawnQueue(dt)
  if #spawnQueue == 0 then
    return
  end
  spawnTimer = spawnTimer - dt
  while spawnTimer <= 0.0 and #spawnQueue > 0 do
    local variant = table.remove(spawnQueue, 1)
    local x, z = pickSpawnPoint()
    AtomF.spawnEnemy(variant, x, z)
    spawnTimer = spawnTimer + SPAWN_STAGGER_SECONDS
  end
end

-- ボスの Hp バー（白 4x4 を並べる）。ダウン中は水色、怒り中は赤。
local WHITE = "./Assets/Images/white4x4.png"
local BAR_SEGMENTS = 32
local BAR_LEFT, BAR_RIGHT, BAR_Y = 0.25, 0.75, 0.07
local SEGMENT_SCALE = 4.5

local function drawBossHp(dt)
  local hp = AtomF.bossHealth()
  if hp < 0 then
    return
  end
  local maxHp = math.max(1, AtomF.bossMaxHealth())
  local bossState = AtomF.bossState()

  -- 表示 Hp は実 Hp へ滑らかに追従（被弾の遅れ表示）
  if bossHpShown == nil then
    bossHpShown = hp
  end
  bossHpShown = bossHpShown + (hp - bossHpShown) * math.min(1.0, dt * 6.0)

  local r, g, b = 0.95, 0.35, 0.85
  if bossState == "staggered" then
    r, g, b = 0.35, 0.8, 1.0
  elseif bossState == "roaring" or bossState == "dying" then
    r, g, b = 1.0, 0.2, 0.2
  elseif hp <= maxHp * 0.5 then
    r, g, b = 1.0, 0.45, 0.25
  end
  local alpha = (bossState == "appearing") and 0.35 or 1.0

  local step = (BAR_RIGHT - BAR_LEFT) / BAR_SEGMENTS
  for i = 0, BAR_SEGMENTS - 1 do
    local x = BAR_LEFT + step * (i + 0.5)
    local segmentHp = maxHp * (i + 1) / BAR_SEGMENTS
    Ui.image(WHITE, x, BAR_Y, { scale = SEGMENT_SCALE, r = 0.12, g = 0.12, b = 0.16, a = 0.6 * alpha })
    if bossHpShown >= segmentHp - maxHp / BAR_SEGMENTS * 0.5 then
      Ui.image(WHITE, x, BAR_Y, { scale = SEGMENT_SCALE * 0.82, r = r, g = g, b = b, a = alpha })
    end
  end
end

-- 毎フレーム呼ぶ。ボス撃破の余韻が済んだら "cleared" を返す。
function M.update(dt)
  drawBossHp(dt)

  local enemyCount = SceneFlow.countByRole("enemy")

  if state == "intro" then
    timer = timer + dt
    if timer >= INTRO_SECONDS then
      state, timer = "wave", 0.0
      waveIndex = 1
      queueWave(waveIndex)
    end

  elseif state == "wave" then
    flushSpawnQueue(dt)
    if #spawnQueue == 0 and enemyCount == 0 then
      timer = timer + dt
      if timer >= WAVE_REST_SECONDS then
        timer = 0.0
        if waveIndex >= #WAVES then
          state = "bossIntro"
        else
          waveIndex = waveIndex + 1
          queueWave(waveIndex)
        end
      end
    else
      timer = 0.0
    end

  elseif state == "bossIntro" then
    timer = timer + dt
    if timer >= BOSS_INTRO_SECONDS then
      state, timer = "boss", 0.0
      AtomF.spawnBoss(0.0, AtomF.arenaRadius() * 0.45)
    end

  elseif state == "boss" then
    local hp = AtomF.bossHealth()
    if hp >= 0 then
      bossSeen = true
    end
    flushSpawnQueue(dt)

    local bossState = AtomF.bossState()
    -- 玉（雑魚）が尽きたら補充する。ボスは雑魚の爆発でしか大ダメージを受けないため。
    if bossSeen and hp > 0 and bossState ~= "appearing" and bossState ~= "dying" and enemyCount <= 1 then
      ammoTimer = ammoTimer + dt
      if ammoTimer >= AMMO_CHECK_SECONDS then
        ammoTimer = 0.0
        for _ = 1, AMMO_COUNT do
          spawnQueue[#spawnQueue + 1] = (math.random() < 0.5) and "walker" or "lurker"
        end
      end
    else
      ammoTimer = 0.0
    end

    if bossSeen and hp < 0 then
      state, timer = "bossDown", 0.0
    end

  elseif state == "bossDown" then
    timer = timer + dt
    if timer >= CLEAR_DELAY_SECONDS then
      return "cleared"
    end
  end

  return nil
end

return M
