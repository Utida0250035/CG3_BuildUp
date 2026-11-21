-- BuildUp: 全方位 STG 自機を JSON プレハブから生成する例
local function onEnter()
  local result = Atrum.spawn_stg_player_from_json()
  if not result.ok then
    Atrum.log("spawn failed: " .. tostring(result.error))
    return
  end
  Atrum.log(result.message or "stg player spawned")
end

return {
  onEnter = onEnter,
}