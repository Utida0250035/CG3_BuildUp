# EditorAssets テンプレート

ゲーム向け Lua スクリプトの雛形。プレースホルダは **`%TokenName%`**（manifest の `tokenDelimiter` 参照）。

## Lua（`Lua/`）

| ID | 用途 |
|----|------|
| `component_draw` | 描画コンポーネント相当の `.lua` |
| `component_upd` | 更新コンポーネント相当の `.lua` |
| `behavior` | 行動スクリプト（Beh 相当）の `.lua` |

`manifest.json` に出力パスと必須トークンを定義。エディタの「スクリプト」パネルから生成する。

### 必須トークン例

- `%ScriptName%` — 英字始まりのモジュール名（`my_component` など）
- `%UpdCmpGroup%` — `MOVEMENT` 等（更新コンポーネント用）

### 任意

- `%AuthorNote%` — 生成時に消すかコメントを残す

出力先: `Project/Assets/Scripts/` 以下。

## 関連

- `EditorAssets/Tools/CursorAI/docs/feature-memos/text_code_templates.md`
