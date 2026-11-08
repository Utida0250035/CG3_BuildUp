# 案内吹き出し（Operator Guidance）

## 用途

- **ランタイム UX:** 一定時間操作していない技を再提示（「しばらく使ってない」）
- **制作:** `EditorAssets/UX/operator_guidance.json` で文言・テクスチャ・表示位置を差し替え

## 実装

| 部品 | 内容 |
|------|------|
| `OperatorGuidance::System` | アクション ID ごとの非使用時間、表示タイマー |
| 描画 | スクリーン空間の吹き出し枠 + `bodyTexturePaths` の画像 |
| 接続 | `CmpMoveByInput` が `parry` を `NotifyAction`、`SceneGame` が Update/Draw |

## データ例

`EditorAssets/UX/operator_guidance.json`

## 今後

- チュートリアル／ナビキャラ差し替え（次 roadmap）
- ビットマップフォントでの `message` 描画
