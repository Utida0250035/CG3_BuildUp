-- atomF 本編の進行（1 ステージ）:
--   ウェーブの雑魚 → ボス登場 → ボス撃破でクリア。自機の Hp が 0 になればゲームオーバー。
--   どちらもリザルトへ遷移する。ウェーブ・ボスの出現はステージ進行役（C++ の
--   CmpAtomFStageDirector）が parameters.json の stage / boss に従って行い、
--   ここでは「ボスが登場し、その後居なくなった」ことを撃破として検知する。
local Paths = require("atomF.scene_paths")
local Outcome = require("atomF.outcome") -- 勝敗をリザルトへ渡す

local M = {}

-- 決着の瞬間（撃破の爆発・被弾演出）を見せてから暗転する。
local WIN_DELAY_SECONDS  = 1.8
local LOSE_DELAY_SECONDS = 1.2

local seenBoss = false
local seenPlayer = false
local endTimer = nil   -- nil: 決着前 / 数値: 決着後の経過秒
local endDelay = 0.0
local flashTime = nil  -- 白フラッシュの経過（nil: なし）
local FLASH_SECONDS = 0.6

local function startFlash()
  flashTime = 0.0
end

-- ボス登場時の警告演出: 画面の上下に赤い帯が走り、点滅しながら引いていく。
local warnTime = nil
local WARN_SECONDS = 2.4

local BOSS_MAX_HEALTH = 128 -- parameters.json の boss.maxHealth と合わせる

-- シーン開始の黒からのフェードイン / 被弾時の赤フラッシュ / 敗北時の赤黒い暗転。
local FADE_IN_SECONDS = 0.8
local HURT_SECONDS    = 0.35
local sceneTime = 0.0
local hurtTime = nil
local lastHealth = nil

local function drawFullScreen(r, g, b, a)
  if a > 0.0 then
    Ui.image(Paths.IMG_WHITE, 0.5, 0.5, { scale = 400.0, r = r, g = g, b = b, a = a })
  end
end

-- シネマ帯: 開始時に上下の黒帯が引いていき、ボス撃破（クリア余韻）では再び寄ってくる。
local INTRO_BAR_SECONDS = 1.4
local BAR_MAX_THICK = 0.12

local function drawCinemaBars(thick)
  if thick <= 0.001 then
    return
  end
  -- Ui.image は一様スケールのため、細かい正方形を横に並べて帯にする
  for i = 0, 24 do
    local x = i / 24.0
    local scale = 4.0 + 60.0 * thick
    Ui.image(Paths.IMG_WHITE, x, thick * 0.5, { scale = scale, r = 0.0, g = 0.0, b = 0.0, a = 1.0 })
    Ui.image(Paths.IMG_WHITE, x, 1.0 - thick * 0.5, { scale = scale, r = 0.0, g = 0.0, b = 0.0, a = 1.0 })
  end
end

local function drawOverlays(dt, playerHealth, bossHealth)
  sceneTime = sceneTime + dt
  local barThick = 0.0
  if sceneTime < INTRO_BAR_SECONDS then
    local k = 1.0 - sceneTime / INTRO_BAR_SECONDS
    barThick = BAR_MAX_THICK * k * k
  end
  if endTimer ~= nil and Outcome.cleared and endDelay > 0.0 then
    local k = math.min(endTimer / (endDelay * 0.5), 1.0)
    barThick = math.max(barThick, BAR_MAX_THICK * k)
  end
  drawCinemaBars(barThick)
  if sceneTime < FADE_IN_SECONDS then
    drawFullScreen(0.0, 0.0, 0.0, 1.0 - sceneTime / FADE_IN_SECONDS)
  end

  if lastHealth ~= nil and playerHealth >= 0 and playerHealth < lastHealth then
    hurtTime = 0.0
  end
  if playerHealth >= 0 then
    lastHealth = playerHealth
  end
  if hurtTime ~= nil then
    hurtTime = hurtTime + dt
    if hurtTime >= HURT_SECONDS then
      hurtTime = nil
    else
      drawFullScreen(1.0, 0.1, 0.1, 0.45 * (1.0 - hurtTime / HURT_SECONDS))
    end
  end

  -- ボスが狂暴化（Hp 40% 以下）している間、画面全体を薄い赤で脈動させて危機感を出す
  if bossHealth > 0 and endTimer == nil and bossHealth / BOSS_MAX_HEALTH <= 0.4 then
    drawFullScreen(0.9, 0.05, 0.05, 0.05 + 0.04 * math.sin(sceneTime * 6.0))
  end

  -- クリアの余韻: 後半に白へ溶けてリザルトへつなぐ
  if endTimer ~= nil and Outcome.cleared and endDelay > 0.0 then
    local t = endTimer / endDelay
    if t > 0.5 then
      drawFullScreen(1.0, 1.0, 1.0, math.min((t - 0.5) * 2.0, 1.0) * 0.9)
    end
  end

  -- 敗北の余韻: 赤黒く沈んでいく
  if endTimer ~= nil and not Outcome.cleared and endDelay > 0.0 then
    drawFullScreen(0.25, 0.0, 0.0, 0.6 * math.min(endTimer / endDelay, 1.0))
  end
end

-- ボスの Hp ゲージ（画面上部）。白 4x4 画像を並べた分割バー。
-- Ui.image は一様スケールのみのため、細かい正方形セグメントで長さを表す。
-- ボス最大 Hp は parameters.json の boss.maxHealth（128）に合わせる。
local BAR_SEGMENTS    = 32
local BAR_SEG_STEP    = 0.0115
local BAR_SEG_SCALE   = 3.4
local BAR_Y           = 0.07
local bossBarAlpha    = 0.0
local bossBarShown    = 0

local function drawBossBar(dt, bossHealth)
  local present = bossHealth >= 0
  -- 登場でフェードイン、消滅でフェードアウト
  local target = present and 1.0 or 0.0
  bossBarAlpha = bossBarAlpha + (target - bossBarAlpha) * math.min(dt * 6.0, 1.0)
  if bossBarAlpha < 0.01 then
    bossBarShown = 0
    return
  end
  if present then
    local ratio = math.max(0.0, math.min(bossHealth / BOSS_MAX_HEALTH, 1.0))
    -- 表示セグメント数は滑らかに追従（減る時だけ少し遅れて見せる）
    local want = ratio * BAR_SEGMENTS
    bossBarShown = bossBarShown + (want - bossBarShown) * math.min(dt * 8.0, 1.0)
  end

  local x0 = 0.5 - (BAR_SEGMENTS - 1) * BAR_SEG_STEP * 0.5
  local enraged = bossBarShown / BAR_SEGMENTS <= 0.4
  for i = 1, BAR_SEGMENTS do
    local x = x0 + (i - 1) * BAR_SEG_STEP
    Ui.image(Paths.IMG_WHITE, x, BAR_Y, { scale = BAR_SEG_SCALE, r = 0.05, g = 0.02, b = 0.08, a = 0.6 * bossBarAlpha })
    local fill = math.max(0.0, math.min(bossBarShown - (i - 1), 1.0))
    if fill > 0.0 then
      local pulse = enraged and (0.8 + 0.2 * math.sin(sceneTime * 12.0)) or 1.0
      if enraged then
        Ui.image(Paths.IMG_WHITE, x, BAR_Y, { scale = BAR_SEG_SCALE * fill, r = 1.0, g = 0.25, b = 0.2, a = pulse * bossBarAlpha })
      else
        Ui.image(Paths.IMG_WHITE, x, BAR_Y, { scale = BAR_SEG_SCALE * fill, r = 0.75, g = 0.3, b = 0.95, a = bossBarAlpha })
      end
    end
  end
end

local function drawWarning(dt)
  if warnTime == nil then
    return
  end
  warnTime = warnTime + dt
  if warnTime >= WARN_SECONDS then
    warnTime = nil
    return
  end
  local t = warnTime / WARN_SECONDS
  local slide = math.min(warnTime / 0.25, 1.0)        -- 最初の 0.25 秒で帯が伸びる
  local fade = t > 0.7 and (1.0 - (t - 0.7) / 0.3) or 1.0
  local blink = 0.65 + 0.35 * math.sin(warnTime * 14.0)
  local a = 0.7 * blink * fade
  local thick = 0.07 * slide
  -- Ui.image は一様スケールのため、細かい正方形を横に並べて帯にする
  for i = 0, 24 do
    local x = i / 24.0
    Ui.image(Paths.IMG_WHITE, x, thick * 0.5, { scale = 4.0 + 60.0 * thick, r = 0.9, g = 0.05, b = 0.05, a = a })
    Ui.image(Paths.IMG_WHITE, x, 1.0 - thick * 0.5, { scale = 4.0 + 60.0 * thick, r = 0.9, g = 0.05, b = 0.05, a = a })
  end
end

local function drawFlash(dt)
  if flashTime == nil then
    return
  end
  flashTime = flashTime + dt
  if flashTime >= FLASH_SECONDS then
    flashTime = nil
    return
  end
  Ui.image(Paths.IMG_WHITE, 0.5, 0.5, { scale = 400.0, a = 0.7 * (1.0 - flashTime / FLASH_SECONDS) })
end

function M.onUpdate(dt)
  drawFlash(dt)
  drawWarning(dt)
  drawBossBar(dt, SceneFlow.healthByRole("boss"))
  drawOverlays(dt, SceneFlow.healthByRole("player"), SceneFlow.healthByRole("boss"))
  if SceneFlow.isChanging() then
    return
  end

  if endTimer ~= nil then
    endTimer = endTimer + dt
    if endTimer >= endDelay then
      SceneFlow.change(Paths.RESULT)
    end
    return
  end

  local bossCount = SceneFlow.countByRole("boss")
  local playerHealth = SceneFlow.healthByRole("player") -- 自機が居なければ -1

  if bossCount > 0 and not seenBoss then
    seenBoss = true
    startFlash() -- ボス登場の白フラッシュ
    warnTime = 0.0 -- 続けて警告の赤帯
  end
  if playerHealth >= 0 then
    seenPlayer = true
  end

  local playerDead = playerHealth == 0 or (seenPlayer and playerHealth < 0)
  local bossDefeated = seenBoss and bossCount == 0

  -- 同時なら相打ちではなく自機の敗北を優先する。
  if playerDead then
    Outcome.cleared = false
    endTimer = 0.0
    endDelay = LOSE_DELAY_SECONDS
  elseif bossDefeated then
    Outcome.cleared = true
    startFlash()
    endTimer = 0.0
    endDelay = WIN_DELAY_SECONDS
  end
end

return M
