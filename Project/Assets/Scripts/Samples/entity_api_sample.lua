-- エンティティ Lua API の使用例（CmpScriptUpd に指定して使う）。型定義は _api/entity.d.lua
local M = {}

---@param self Entity
function M.onResolve(self)
  print("[entity_api_sample] components: " .. table.concat(self:components(), ", "))
end

---@param dt number
---@param self Entity
function M.onUpdate(dt, self)
  -- 毎フレーム X 方向へ移動
  self:translate(Vec3(1.0 * dt, 0, 0))

  -- 体力 0 のエンティティを探して破棄
  for _, e in ipairs(Entities.findByTag("enemy")) do
    if e:hasHealth() and e:getHealth() == 0 then
      e:destroy()
    end
  end
end

return M
