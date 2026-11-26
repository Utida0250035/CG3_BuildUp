# モーションブレンディング

## 用途

- **ランタイム:** 2 つのモーションクリップを重み付け合成し、簡易キャラ／プロップの Transform を駆動
- **制作:** `EditorAssets/Motion/*.json` でキーフレーム編集

## API

| 型 | 役割 |
|----|------|
| `MotionClip` | キーフレーム列 + duration |
| `SampleMotionClip` | クリップ内補間（キーフレーム間は Slerp / Lerp） |
| `BlendMotionTransforms` | クリップ A/B のポーズを `weight` で混合 |
| `CmpMotionBlend` | Entity に適用する `ComponentUpd` |

## コンポーネント例

```cpp
auto *motion = entity->AddUpdCmp<CmpMotionBlend>();
motion->SetClipPaths("../EditorAssets/Motion/sample_motion_a.json",
                     "../EditorAssets/Motion/sample_motion_b.json");
motion->SetBlendWeight(0.35f);
```

## 関連

- クリップ間遷移: `motion_interpolation.md`（`MotionCrossfade` / `CmpMotionCrossfade`）
