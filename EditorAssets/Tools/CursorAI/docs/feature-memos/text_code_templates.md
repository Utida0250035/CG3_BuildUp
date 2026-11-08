# テキスト系 %変数名% テンプレ（テンプレファイル）

## 推測した用途

- **レベル／ゲームロジック制作:** 新コンポーネント・行動クラスを命名規約どおりに素早く足す
- **エディタ統合（次タスク）:** manifest 駆動でファイル生成・vcxproj 追記候補を提示

## 配置

`EditorAssets/Templates/Code/`

- `manifest.json` — テンプレ ID、出力相対パス、必須トークン
- `*.template` — 本体（`%ClassName%` 等）

## トークン規約

| トークン | 意味 |
|----------|------|
| `%ClassName%` | C++ 型名（`Cmp` 接頭を含むことを推奨） |
| `%UpdCmpGroup%` | `UpdCmpGroup` 列挙子名 |
| `%AuthorNote%` | 任意コメント（空文字で置換可） |

## 次 roadmap

- 作成・コピー・置換コード（Python または Editor C++）
- ImGui からの生成 UI
