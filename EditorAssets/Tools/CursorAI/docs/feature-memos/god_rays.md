# ゴッドレイ（ライトシャフト）

## 用途

- **ランタイム:** 太陽／平行光方向を見たときの光の筋で屋外の雰囲気を強調
- **レベル制作:** `VolumetricFog` と連動（`fogCoupling`）して霧の濃さに応じた見え方

## 実装（現段階）

| 層 | 内容 |
|----|------|
| `GodRays` / `GodRaySettings` | `Lighting`・`AtrumEngine` で `DirectionalLightData` へ反映 |
| `Object3d.PS` | 視線と `-lightDir` の角度に基づくシャフト加算（フォグ媒質で減衰） |

ポストプロセスの放射ブラーは未実装。将来 `EditorSceneRenderTarget` 経由のフルスクリーンパスへ移行可能。

## パラメータ

`intensity`, `angularPower`, `fogCoupling`, `color`, `enabled`

## 既定

`SceneGame::SetupGameLighting` でフォグ有効時に弱いゴッドレイをオン。
