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

### サブシステム API（`Atrum.Command` / `Audio` / `Storage` / `Registry`）

| 名前空間 | 用途例 |
|----------|--------|
| `Atrum.Command.Scene` | 現シーン有無 |
| `Atrum.Command.ChangeScene` | 遷移中判定、`queue_buildup_scene(path)` |
| `Atrum.Command.Context` | `CommandContext`（D3D12 リスト種別の参照のみ） |
| `Atrum.Audio` | `play(path)`, `stop(handle)`, `play_scene_bgm(path)` |
| `Atrum.Storage.Entity` | `EntityStorage` / シーン内 Entity 検索 |
| `Atrum.Storage.Texture` | `TextureStorage` インデックス・寸法 |
| `Atrum.Storage.Model` | `ModelStorage` 経由の OBJ 取得 |
| `Atrum.Registry.EntityTag` | タグ registry（`tag_for_role` 等） |
| `Atrum.Registry.Cmp` | `RegistryCmp` の `give_upd` / `has_upd` |
| `Atrum.Registry.SceneBuildUp` / `SceneMovie` | 各 Registry の `create_owned` |
| `Atrum.Registry.EditorComponent` | エディタ既知コンポーネント型の照会 |

`Atrum.tag_for_role` は後方互換のため残しています（中身は `Registry.EntityTag` と同系）。
