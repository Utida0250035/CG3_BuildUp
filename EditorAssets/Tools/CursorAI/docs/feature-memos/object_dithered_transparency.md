# オブジェクトの粒々透過（Dithered Alpha）

## 用途

- **ランタイム:** 半透明オブジェクトをソートなしで描画（ステージ上のフェンス・ガラス風メッシュ等）
- **見た目:** 4×4 Bayer 閾値で `clip` し、粒状の透過表現

## 実装

| 要素 | 内容 |
|------|------|
| `BlendMode::DITHERED_ALPHA` | 深度書き込み ON・ブレンド OFF PSO |
| `Object3d.PS.hlsl` | `USE_DITHER_CLIP` でスクリーン空間ディザ |
| `CmpMesh` | `color.w < 1` のとき `DITHERED_ALPHA` を選択 |

## 使い方（コード）

```cpp
engine->SetBlendModeForFrame(BlendMode::DITHERED_ALPHA);
engine->DrawModel(...);
engine->SetBlendModeForFrame(BlendMode::NONE);
```

`BINARY_ALPHA` は従来どおりアルファ 0.5 固定 clip。

## 制限

- 粒々パターンは画面固定（カメラ移動でちらつく場合あり）
- 真の半透明グラデーションには `NORMAL` ブレンドが必要（別途ソート）
