-- atomF リザルトシーン: 「リザルト」テキストと「リトライ」「タイトルへ」の選択 UI。
-- Space / A で決定。リトライ→本編、タイトルへ→タイトル。
local Paths = require("atomF.scene_paths")
local Outcome = require("atomF.outcome")

local M = {}

local INPUT_LOCKOUT_SECONDS = 0.6   -- 本編の連打（Space）で即決定されないように
local FADE_IN_SECONDS       = 0.6
local DECIDE_FLASH_SECONDS  = 0.3
local STICK_THRESHOLD       = 0.6

local OPTIONS = {
  { image = Paths.IMG_RETRY,    target = Paths.GAME,  y = 0.58 },
  { image = Paths.IMG_TO_TITLE, target = Paths.TITLE, y = 0.73 },
}

local elapsed = 0.0
local selected = 1
local decidedAt = nil
local stickWasTilted = false

local function moveSelection(delta)
  selected = ((selected - 1 + delta) % #OPTIONS) + 1
end

local function pollSelectionInput()
  if Input.upTrigger() then
    moveSelection(-1)
  elseif Input.downTrigger() then
    moveSelection(1)
  end

  -- 左スティックの上下（倒した瞬間だけ 1 回移動）
  local y = Input.stickY()
  local tilted = math.abs(y) >= STICK_THRESHOLD
  if tilted and not stickWasTilted then
    moveSelection(y > 0 and -1 or 1)
  end
  stickWasTilted = tilted
end

function M.onUpdate(dt)
  elapsed = elapsed + dt
  local alpha = math.min(elapsed / FADE_IN_SECONDS, 1.0)

  if decidedAt == nil and elapsed >= INPUT_LOCKOUT_SECONDS then
    pollSelectionInput()
    if Input.confirmTrigger() then
      decidedAt = elapsed
    end
  end

  -- 見出しは大きく出現して収まる（オーバーシュート）。クリア時はその後ゆるく脈動する
  local pop = math.min(elapsed / FADE_IN_SECONDS, 1.0)
  local headScale = 1.0 + 0.6 * (1.0 - pop) * (1.0 - pop) * math.cos(pop * 6.0)
  local headY = 0.25
  if Outcome.cleared then
    headScale = headScale + 0.03 * math.sin(elapsed * 2.5)
    headY = headY - 0.008 * math.sin(elapsed * 2.0)
  else
    -- 敗北時は出現直後だけ小刻みに横ブレして沈む
    headY = headY + 0.02 * (1.0 - pop)
  end

  -- 勝敗で見出しを切り替える（クリア: 金色 / 敗北: 赤）
  if Outcome.cleared then
    Ui.image(Paths.IMG_CLEAR, 0.5, headY, { scale = headScale, r = 1.0, g = 0.88, b = 0.35, a = alpha })
  else
    local shake = 0.012 * (1.0 - pop) * math.sin(elapsed * 60.0)
    Ui.image(Paths.IMG_GAME_OVER, 0.5 + shake, headY, { scale = headScale, r = 1.0, g = 0.3, b = 0.3, a = alpha })
  end

  for index, option in ipairs(OPTIONS) do
    local isSelected = index == selected
    local scale = 1.0
    local r, g, b, a = 0.6, 0.6, 0.6, 0.8

    if isSelected then
      -- 選択中は黄色く、ゆっくり脈動させる
      scale = 1.12 + 0.04 * math.sin(elapsed * 6.0)
      r, g, b, a = 1.0, 0.9, 0.35, 1.0
      if decidedAt ~= nil then
        local t = elapsed - decidedAt
        a = (math.floor(t * 20.0) % 2 == 0) and 1.0 or 0.25
      end
    end

    Ui.image(option.image, 0.5, option.y, { scale = scale, r = r, g = g, b = b, a = a * alpha })
  end

  if decidedAt ~= nil and elapsed - decidedAt >= DECIDE_FLASH_SECONDS then
    SceneFlow.change(OPTIONS[selected].target)
  end
end

return M
