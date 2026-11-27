-- atomF 本編の進行（1 ステージ）:
--   ウェーブの雑魚 → ボス登場 → ボス撃破でクリア。自機の Hp が 0 になればゲームオーバー。
--   どちらもリザルトへ遷移する。ウェーブ・ボスの出現はステージ進行役（C++ の
--   CmpAtomFStageDirector）が parameters.json の stage / boss に従って行い、
--   ここでは「ボスが登場し、その後居なくなった」ことを撃破として検知する。
local Paths = require("atomF.scene_paths")

local M = {}

-- 決着の瞬間（撃破の爆発・被弾演出）を見せてから暗転する。
local WIN_DELAY_SECONDS  = 1.8
local LOSE_DELAY_SECONDS = 1.2

local seenBoss = false
local seenPlayer = false
local endTimer = nil   -- nil: 決着前 / 数値: 決着後の経過秒
local endDelay = 0.0

function M.onUpdate(dt)
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

  if bossCount > 0 then
    seenBoss = true
  end
  if playerHealth >= 0 then
    seenPlayer = true
  end

  local playerDead = playerHealth == 0 or (seenPlayer and playerHealth < 0)
  local bossDefeated = seenBoss and bossCount == 0

  -- 同時なら相打ちではなく自機の敗北を優先する。
  if playerDead then
    endTimer = 0.0
    endDelay = LOSE_DELAY_SECONDS
  elseif bossDefeated then
    endTimer = 0.0
    endDelay = WIN_DELAY_SECONDS
  end
end

return M
