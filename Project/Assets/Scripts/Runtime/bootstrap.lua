-- SceneBuildUp シーン入場時に自動実行（存在する場合のみ）
Atrum.log("Lua runtime bootstrap")

-- 例: 既存エンティティへ Lua コンポーネントを付与
-- Atrum.attach_lua_upd("entity_editor_preview", "Assets/Scripts/Components/Upd/example_upd.lua", "MOVEMENT")

-- 例: 行動スクリプトを登録する場合
-- Atrum.require_behavior("Assets/Scripts/Behaviors/my_behavior.lua")

-- 例: プレハブ JSON から Entity を生成する場合
-- local result = Atrum.spawn_entity_from_json("../EditorAssets/Entities/your_entity.json", 0, 2, 0)
-- if not result.ok then
--   Atrum.log("spawn failed: " .. (result.error or "?"))
-- end