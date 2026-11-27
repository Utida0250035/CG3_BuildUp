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

local function drawOverlays(dt, playerHealth)
  sceneTime = sceneTime + dt
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

  -- 敗北の余韻: 赤黒く沈んでいく
  if endTimer ~= nil and not Outcome.cleared and endDelay > 0.0 then
    drawFullScreen(0.25, 0.0, 0.0, 0.6 * math.min(endTimer / endDelay, 1.0))
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
  drawOverlays(dt, SceneFlow.healthByRole("player"))
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
