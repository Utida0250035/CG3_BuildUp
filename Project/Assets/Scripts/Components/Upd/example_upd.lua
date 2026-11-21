-- サンプル Lua 更新コンポーネント（CmpLuaUpd）
local M = {}

M.scriptName = "example_upd"
M.updCmpGroup = "MOVEMENT"

function M.onResolve(ctx)
  Atrum.log("example_upd onResolve entityId=" .. tostring(ctx.entityId))
end

function M.onUpdate(dt, ctx)
  -- ランタイムでは Entity の Transform を Lua から直接書き換える API は未実装
  if ctx and ctx.position then
    -- デバッグ用: 初回のみログ（毎フレームは重い）
  end
end

return M