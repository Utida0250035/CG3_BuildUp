-- atomF タイトルシーン: ロゴと「Press any to start」を中央揃えで表示し、
-- 何かキー／ボタンを押したら本編へ遷移する。
local Paths = require("atomF.scene_paths")

local M = {}

local INPUT_LOCKOUT_SECONDS = 0.3   -- シーン遷移直後の押しっぱなしを無視
local LOGO_FADE_IN_SECONDS  = 0.8
local START_FLASH_SECONDS   = 0.35  -- 押下後、フェードアウト前の点滅演出

local elapsed = 0.0
local startedAt = nil -- 押下した時刻（nil なら未押下）

function M.onUpdate(dt)
  elapsed = elapsed + dt

  -- ロゴ: フェードインしつつ、押下後は少し拡大して弾ける
  local logoAlpha = math.min(elapsed / LOGO_FADE_IN_SECONDS, 1.0)
  local logoScale = 1.0
  local pressAlpha
  local pressColorG = 1.0

  if startedAt == nil then
    -- ゆっくり明滅
    pressAlpha = 0.55 + 0.45 * math.sin(elapsed * 3.0)
  else
    local t = elapsed - startedAt
    logoScale = 1.0 + 0.08 * math.min(t / START_FLASH_SECONDS, 1.0)
    -- 高速点滅（水色寄りに光らせる）
    pressAlpha = (math.floor(t * 20.0) % 2 == 0) and 1.0 or 0.25
    pressColorG = 1.0
  end

  local logoY = 0.38 + 0.008 * math.sin(elapsed * 1.6) -- ゆるく上下に漂う
  Ui.image(Paths.IMG_TITLE_LOGO, 0.5, logoY, { scale = logoScale, a = logoAlpha })
  Ui.image(Paths.IMG_PRESS_ANY, 0.5, 0.72, { a = pressAlpha * logoAlpha, g = pressColorG })

  if startedAt == nil then
    if elapsed >= INPUT_LOCKOUT_SECONDS and Input.anyTrigger() then
      startedAt = elapsed
    end
  elseif elapsed - startedAt >= START_FLASH_SECONDS then
    -- 遷移演出（暗転フェード）は SceneFlow.change が開始する
    SceneFlow.change(Paths.GAME)
  end
end

return M
