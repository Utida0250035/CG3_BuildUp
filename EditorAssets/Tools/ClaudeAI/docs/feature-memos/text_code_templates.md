# テキスト系 %変数名% テンプレ（Lua スクリプト）

## 推測した用途

- **レベル／ゲームロジック制作:** Draw / Upd / Beh 相当の Lua 雛形を命名規約どおりに素早く足す
- **エディタ統合:** manifest 駆動で `Project/Assets/Scripts/` へファイル生成（Phase 1）。ランタイム読込は Phase 2

## 配置

`EditorAssets/Templates/Lua/`

- `manifest.json` — テンプレ ID、出力相対パス、必須トークン
- `*.lua.template` — 本体（`%ScriptName%` 等）

## トークン規約

| トークン | 意味 |
|----------|------|
| `%ScriptName%` | Lua モジュール名（英字始まり・英数字と `_`） |
| `%UpdCmpGroup%` | C++ `UpdCmpGroup` 列挙子名（更新雛形のみ） |
| `%AuthorNote%` | 任意コメント（空で行ごと削除） |

## エディタ（DEVELOPMENT + USE_IMGUI）

- メニュー **エディタ → スクリプト**、ドック **スクリプト**
- `EditorLuaScriptScaffold::DrawImGuiPanel()` — manifest 読み込みと生成
