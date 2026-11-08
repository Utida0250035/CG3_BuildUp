# ボリュームフォグ（VolumetricFog 統合）

## 用途

- **ランタイム:** 屋外・床面付近の遠景を霧で馴染ませ、シルエットと奥行きを読みやすくする
- **レベル制作:** シーン／`Visual::Lighting` と `AtrumEngine` で同一 `VolumetricFogSettings` を共有

## 実装

| 層 | 内容 |
|----|------|
| `VolumetricFog` | 設定の単一ソース。`ApplyTo` で `DirectionalLightData` / `LightDataDirectional` へ反映 |
| `Lighting` | `RefVolumetricFog()`、`Update` 時に新ライト CB へ統合 |
| `AtrumEngine` | `RefVolumetricFog()`、`FlushDirectionalLightConstantBuffer` で現行 3D 描画 CB へ反映 |
| `Object3d.PS` | 距離×高さの指数フォグ + 平行光インスキャッター |

## パラメータ

`density`, `heightFalloff`, `heightOffset`, `startDistance`, `maxDistance`, `inScatterIntensity`, `color`, `enabled`

## 既定

`SceneGame::SetupGameLighting` で弱いフォグを有効化。

## 今後

ゴッドレイ／フロクセルボリュームへ拡張する際は `VolumetricFog` を軸に GPU パスを追加。
