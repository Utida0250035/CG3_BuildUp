-- atomF 本編の進行: ステージ（雑魚ウェーブ→ボス）を進め、ボス撃破または自機の Hp 0 でリザルトへ遷移する。
local Paths = require("atomF.scene_paths")
local Stage = require("atomF.stage")

local M = {}

local END_DELAY_SECONDS = 1.0 -- 自機撃墜の瞬間を見せてから暗転する

local seenPlayer = false
local endTimer = nil -- nil: 決着前 / 数値: 決着後の経過秒

-- シーン入場のたびに状態を戻す（require 済みモジュールの状態は持ち越されるため）
function M.onResolve()
  seenPlayer = false
  endTimer = nil
  Stage.reset()
end

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

  local playerHealth = SceneFlow.healthByRole("player") -- 自機が居なければ -1
  if playerHealth >= 0 then
    seenPlayer = true
  end
  local playerDead = playerHealth == 0 or (seenPlayer and playerHealth < 0)
  if playerDead then
    endTimer = 0.0
    return
  end

  if Stage.update(dt) == "cleared" then
    endTimer = END_DELAY_SECONDS -- 余韻はステージ側で済んでいるので即遷移
  end
end

return M
