# ボリュームフォグ（VolumetricFog 統合）

## 用途

- **ランタイム:** 屋外・床面付近の遠景を霧で馴染ませ、シルエットと奥行きを読みやすくする
- **レベル制作:** シーン／`Visual::Lighting` と `AtrumEngine` で同一 `VolumetricFogSettings` を共有

## 実装

| 層 | 内容 |
|----|------|
| `VolumetricFog` | 設定オブジェクト。`ApplyTo` で `DirectionalLightData` へ反映 |
| `AtrumEngine` | **単一正本** … `RefVolumetricFog()` と `FlushDirectionalLightConstantBuffer` |
| `Visual::Lighting` | ファサード … `RefVolumetricFog()` は `LightingEngineEffectsBinding` 経由で Engine と同一インスタンス |
| `Object3d.PS` | 距離×高さの指数フォグ + 平行光インスキャッター |

## パラメータ

`density`, `heightFalloff`, `heightOffset`, `startDistance`, `maxDistance`, `inScatterIntensity`, `color`, `enabled`

## 既定

`SceneGame::SetupGameLighting` で弱いフォグを有効化。

## 今後

ゴッドレイ／フロクセルボリュームへ拡張する際は `VolumetricFog` を軸に GPU パスを追加。
