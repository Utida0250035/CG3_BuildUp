# EditorAssets テンプレート

ゲーム向けコード・データの雛形。プレースホルダは **`%TokenName%`**（manifest の `tokenDelimiter` 参照）。

## Code（`Code/`）

| ID | 用途 |
|----|------|
| `component_draw` | 新規 `ComponentDraw` の .h / .cpp |
| `component_upd` | 新規 `ComponentUpd` の .h / .cpp |
| `behavior` | `Beh*` 系ロジックの雛形 |

`manifest.json` に出力パスと必須トークンを定義。置換・コピーは roadmap「作成・コピー・置換コード」で実装予定。

### 必須トークン例

- `%ClassName%` — `CmpMyFeature` のような PascalCase クラス名
- `%UpdCmpGroup%` — `MOVEMENT` / `DEFAULT` 等（`ComponentUpd` 用）

### 任意

- `%AuthorNote%` — 生成時に消すかコメントを残す

## 関連

- `EditorAssets/Tools/CursorAI/docs/feature-memos/text_code_templates.md`
