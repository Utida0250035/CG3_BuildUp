# Scripts（Lua）

エディタの「スクリプト」パネルから生成した LuaJIT 用スクリプトを配置します。

- `Components/Draw/` — 描画コンポーネント雛形
- `Components/Upd/` — 更新コンポーネント雛形
- `Behaviors/` — 行動スクリプト雛形

`SceneBuildUp` シーン入場時に `Runtime/bootstrap.lua` があれば自動実行されます。
`Atrum.spawn_entity_from_json` で `EditorAssets/Entities` などの JSON から Entity を追記生成できます。
