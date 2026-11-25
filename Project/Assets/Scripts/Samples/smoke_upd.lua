-- エディタプレビュー動作確認用（CmpScriptUpd）
local M = {}

function M.onResolve()
  print("[smoke_upd] onResolve")
end

function M.onUpdate(dt)
  print(string.format("[smoke_upd] onUpdate dt=%.4f", dt))
end

return M