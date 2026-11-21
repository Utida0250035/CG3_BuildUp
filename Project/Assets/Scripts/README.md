# Scripts（Lua）

エディタの「スクリプト」パネルから生成した LuaJIT 用スクリプトを配置します。

- `Components/Draw/` — 描画コンポーネント雛形
- `Components/Upd/` — 更新コンポーネント雛形
- `Behaviors/` — 行動スクリプト雛形

`SceneBuildUp` シーン入場時に `Runtime/bootstrap.lua` があれば自動実行されます。

- `Atrum.spawn_entity_from_json` — JSON プレハブから Entity を追記
- `Atrum.attach_lua_upd(entityName, scriptPath, updCmpGroup?)` — Lua 更新コンポーネントを実行時に付与
- `Atrum.attach_lua_draw(entityName, scriptPath, drawCmpGroup?)` — Lua 描画コンポーネントを実行時に付与
- エンティティ JSON のコンポーネントスロット `CmpLuaUpd` / `CmpLuaDraw` — `scriptPath` で雛形を指定
