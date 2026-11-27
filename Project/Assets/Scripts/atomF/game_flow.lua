-- atomF 本編（ステージ 1）の進行:
--   ウェーブ 1〜4（雑魚の組み合わせ）→ ボス警告 → ボス戦 → クリア。自機の Hp が 0 で敗北。
-- どちらの終わり方でもリザルトへ遷移し、結果は Outcome で渡す。
-- 敵・ボスの挙動は C++ コンポーネント＋ parameters.json。ここは「いつ何を出すか」と HUD だけを持つ。
local Paths   = require("atomF.scene_paths")
local Stage   = require("atomF.stage_data")
local Outcome = require("atomF.outcome")

local M = {}

local TWO_PI = 6.28318530718
local BANNER_SECONDS = 2.2
local INTRO_SECONDS = 1.6
local PIP_COUNT = 32
local PIP_SCALE = 3.0           -- 4px * 3 = 12px
local PIP_PITCH_PX = 14.0
local SCREEN_W = 1280.0         -- 既定ウィンドウ幅（HUD の横位置を px から割合へ直す用）

-- phase: intro → spawning → fighting → (interval) → … → bossWarning → boss → ending
local phase = "intro"
local phaseTime = 0.0
local waveIndex = 0
local pending = {}              -- 湧き待ちの { variant, x, z }
local spawnTimer = 0.0
local waveTime = 0.0
local clearedTime = nil         -- 敵が居なくなってからの経過（nil: まだ居る）
local playerSeen = false
local bossSeen = false
local flashTime = nil           -- 白フラッシュの経過
local endTimer = nil            -- 決着後の経過（nil: 決着前）

local function setPhase(name)
  phase = name
  phaseTime = 0.0
end

local function flash()
  flashTime = 0.0
end

-- 湧き位置: 円周に等間隔。自機に近い点は反対側へずらす。
local function buildWave(index)
  local wave = Stage.waves[index]
  local list = {}
  for _, entry in ipairs(wave.spawns) do
    for _ = 1, entry[2] do
      list[#list + 1] = entry[1]
    end
  end

  local radius = AtomF.arenaRadius() * Stage.spawnRadiusRatio
  local px, pz = AtomF.playerPosition()
  local baseAngle = index * 0.9
  pending = {}
  for i, variant in ipairs(list) do
    local angle = baseAngle + TWO_PI * (i - 1) / #list
    local x, z = math.cos(angle) * radius, math.sin(angle) * radius
    if px ~= nil then
      local dx, dz = x - px, z - pz
      if math.sqrt(dx * dx + dz * dz) < Stage.playerSafeRadius then
        x, z = -x, -z
      end
    end
    pending[#pending + 1] = { variant = variant, x = x, z = z }
  end
  spawnTimer = 0.0
  waveTime = 0.0
  clearedTime = nil
end

local function startWave(index)
  waveIndex = index
  buildWave(index)
  setPhase("spawning")
end

local function bossSpawnPoint()
  -- 自機から最も遠い側（反対側）へ。居なければ北側。
  local px, pz = AtomF.playerPosition()
  if px == nil or (px * px + pz * pz) < 1.0 then
    return 0.0, AtomF.arenaRadius() * 0.5
  end
  local len = math.sqrt(px * px + pz * pz)
  local r = AtomF.arenaRadius() * 0.5
  return -px / len * r, -pz / len * r
end

local function finish(cleared)
  if endTimer ~= nil then
    return
  end
  Outcome.cleared = cleared
  endTimer = 0.0
  if cleared then
    flash()
  end
end

-- ---- HUD ----------------------------------------------------------------

local function drawBanner(image, t, duration, y, r, g, b)
  -- フェードイン → 保持 → フェードアウト
  local a = 1.0
  local fade = 0.35
  if t < fade then
    a = t / fade
  elseif t > duration - fade then
    a = math.max(0.0, (duration - t) / fade)
  end
  local scale = 1.0 + 0.25 * math.max(0.0, 1.0 - t / 0.5) -- 出だしだけ少し大きい
  Ui.image(image, 0.5, y, { scale = scale, a = a, r = r, g = g, b = b })
end

local function drawBossGauge(dt)
  local hp = AtomF.bossHealth()
  if hp < 0 then
    return
  end
  local maxHp = AtomF.bossMaxHealth()
  local filled = math.ceil(PIP_COUNT * hp / maxHp)
  local phase2 = AtomF.bossPhase2()
  local stunned = AtomF.bossState() == "stunned"
  local pulse = 0.5 + 0.5 * math.sin(phaseTime * 14.0)

  Ui.image(Paths.IMG_BOSS_NAME, 0.5, 0.045, { scale = 0.55, r = 0.85, g = 0.6, b = 1.0, a = 0.95 })

  local startPx = -(PIP_COUNT - 1) * PIP_PITCH_PX * 0.5
  for i = 1, PIP_COUNT do
    local x = 0.5 + (startPx + (i - 1) * PIP_PITCH_PX) / SCREEN_W
    -- 空きマス
    Ui.image(Paths.IMG_WHITE, x, 0.095, { scale = PIP_SCALE, r = 0.15, g = 0.1, b = 0.2, a = 0.7 })
    if i <= filled then
      local r, g, b = 0.75, 0.3, 1.0
      if phase2 then r, g, b = 1.0, 0.3, 0.3 end
      if stunned then r, g, b = 0.6 + 0.4 * pulse, 0.9, 1.0 end
      Ui.image(Paths.IMG_WHITE, x, 0.095, { scale = PIP_SCALE - 0.5, r = r, g = g, b = b, a = 1.0 })
    end
  end
end

local function drawFlash(dt)
  if flashTime == nil then
    return
  end
  flashTime = flashTime + dt
  local duration = 0.6
  if flashTime >= duration then
    flashTime = nil
    return
  end
  Ui.image(Paths.IMG_WHITE, 0.5, 0.5, { scale = 400.0, a = 0.7 * (1.0 - flashTime / duration) })
end

local function drawWarning()
  -- 画面を赤く点滅させ、WARNING を明滅させる
  local pulse = 0.5 + 0.5 * math.sin(phaseTime * 9.0)
  Ui.image(Paths.IMG_WHITE, 0.5, 0.5, { scale = 400.0, r = 1.0, g = 0.0, b = 0.0, a = 0.10 + 0.14 * pulse })
  local a = 0.5 + 0.5 * pulse
  if phaseTime < 0.3 then a = a * phaseTime / 0.3 end
  Ui.image(Paths.IMG_WARNING, 0.5, 0.42, { scale = 1.0 + 0.04 * pulse, r = 1.0, g = 0.25, b = 0.2, a = a })
end

-- ---- 進行 ---------------------------------------------------------------

function M.onUpdate(dt)
  phaseTime = phaseTime + dt
  drawFlash(dt)

  if SceneFlow.isChanging() then
    return
  end

  local playerHealth = SceneFlow.healthByRole("player") -- 自機が居なければ -1
  if playerHealth >= 0 then
    playerSeen = true
  end

  -- 決着後は暗転までの待ち
  if endTimer ~= nil then
    endTimer = endTimer + dt
    if endTimer >= Stage.endDelaySeconds then
      SceneFlow.change(Paths.RESULT)
    end
    return
  end

  if playerHealth == 0 or (playerSeen and playerHealth < 0) then
    finish(false)
    return
  end

  local enemyCount = SceneFlow.countByRole("enemy")

  if phase == "intro" then
    if phaseTime < BANNER_SECONDS then
      drawBanner(Paths.IMG_WAVES[1], phaseTime, BANNER_SECONDS, 0.3, 1.0, 1.0, 1.0)
    end
    if phaseTime >= INTRO_SECONDS then
      startWave(1)
    end

  elseif phase == "spawning" or phase == "fighting" then
    waveTime = waveTime + dt
    if waveIndex > 1 and waveTime < BANNER_SECONDS then
      drawBanner(Paths.IMG_WAVES[waveIndex], waveTime, BANNER_SECONDS, 0.3, 1.0, 1.0, 1.0)
    end

    if phase == "spawning" then
      spawnTimer = spawnTimer - dt
      if spawnTimer <= 0.0 and #pending > 0 then
        local s = table.remove(pending, 1)
        AtomF.spawnEnemy(s.variant, s.x, s.z)
        spawnTimer = Stage.spawnIntervalSeconds
      end
      if #pending == 0 then
        setPhase("fighting")
      end
    else
      -- 全滅、または時間切れで残りが少なければ次へ
      local timedOut = waveTime >= Stage.waveTimeoutSeconds and enemyCount <= Stage.waveTimeoutRemaining
      if enemyCount == 0 or timedOut then
        clearedTime = (clearedTime or 0.0) + dt
        if clearedTime >= Stage.clearIntervalSeconds or timedOut then
          if waveIndex < #Stage.waves then
            startWave(waveIndex + 1)
          else
            setPhase("bossWarning")
          end
        end
      else
        clearedTime = nil
      end
    end

  elseif phase == "bossWarning" then
    drawWarning()
    if phaseTime >= Stage.bossWarningSeconds then
      local x, z = bossSpawnPoint()
      AtomF.spawnBoss(x, z)
      flash()
      setPhase("boss")
    end

  elseif phase == "boss" then
    if AtomF.bossState() ~= "none" then
      bossSeen = true
    end
    drawBossGauge(dt)
    -- ボスが消え（撃破演出まで終わり）、場の敵も居なくなったらクリア
    if bossSeen and AtomF.bossState() == "none" and enemyCount == 0 then
      finish(true)
    end
  end
end

return M
