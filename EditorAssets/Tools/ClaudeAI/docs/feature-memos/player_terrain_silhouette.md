# 地形越し自機シルエット

## 用途

- **ランタイム:** 手前景（雲・地形メッシュ）に隠れた自機位置を、オデッセイ的に伝える
- **描画タイミング:** `SceneGame::DrawEntities` の最後（前景描画後）

## 実装

| 要素 | 内容 |
|------|------|
| `BlendMode::OCCLUDED_SILHOUETTE` | `DepthFunc = GREATER`、深度書き込みなし、αブレンド |
| `PlayerTerrainSilhouette::DrawOccludedSilhouette` | 自機と同じ球形状・シアン半透明 |
| 呼び出し | 敗北演出中はスキップ |

## 原理

通常パスで書き込まれた深度より **奥** にあるピクセルだけ通すため、遮蔽されている区域にシルエットが乗る。

## 調整

- 色・半径: `PlayerTerrainSilhouette.cpp`
- 自機半径の正本: `kParryColliderRadius`
