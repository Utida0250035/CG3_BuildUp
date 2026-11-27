-- atomF 本編の進行: ボスを倒す、または自機の Hp が 0 になったらリザルトへ遷移する。
-- 雑魚が全滅するとボスが現れる（C++ の CmpAtomFStageDirector）。ボスが現れる前は勝利にしない。
local Paths = require("atomF.scene_paths")

local M = {}

local END_DELAY_SECONDS = 1.0 -- 決着の瞬間を見せてから暗転する

local seenBoss = false
local seenPlayer = false
local endTimer = nil -- nil: 決着前 / 数値: 決着後の経過秒

function M.onUpdate(dt)
  if SceneFlow.isChanging() then
    return
  end

  if endTimer ~= nil then
    endTimer = endTimer + dt
    if endTimer >= END_DELAY_SECONDS then
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

  local bossDefeated = seenBoss and bossCount == 0
  local playerDead = playerHealth == 0 or (seenPlayer and playerHealth < 0)

  if bossDefeated or playerDead then
    endTimer = 0.0
  end
end

return M
