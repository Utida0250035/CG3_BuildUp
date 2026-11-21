-- サンプル Lua 描画コンポーネント（CmpLuaDraw）
local M = {}

M.scriptName = "example_draw"

function M.onResolve(ctx)
  Atrum.log("example_draw onResolve")
end

function M.onDraw(ctx)
  -- 描画 API の Lua バインドは今後拡張予定
end

return M