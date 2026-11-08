# リムライト（Fresnel 縁取り）

## 用途

- **ランタイム描画:** 3D キャラ・小物のシルエットを読みやすくする（ゲームプレイ・アクション）
- **カットシーン:** `SceneMovie` 等でも `DirectionalLightData` を拡張すれば同様に利用可能

## 実装

| 場所 | 内容 |
|------|------|
| `LightData.h` | `DirectionalLightData` / `LightDataDirectional` に rim パラメータ + `cameraWorldPosition` |
| `Object3d.PS.hlsl` | `rim = pow(1 - N·V, power) * intensity` を加算 |
| `Object3d.VS.hlsl` | `worldPosition` を PS へ |
| `AtrumEngine` | 毎フレーム `FlushDirectionalLightConstantBuffer()` でカメラ位置を同期 |

## パラメータ

- `rimEnabled` / `rimColor` / `rimIntensity` / `rimPower`
- `cameraWorldPosition` はエンジンが自動設定（手動設定不要）

## 既定例

`SceneGame::SetupGameLighting` で弱めの青白リムを有効化。

## ビルド

Windows `DEVELOPMENT` でシェーダ再コンパイル（`ShaderCompiler` 経由）が必要。
