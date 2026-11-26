# モーション間の補間（クロスフェード）

## 用途

- **ランタイム:** モーション A 再生中に B へ滑らかに切り替え（スケルタルなし Transform 駆動）
- **演出:** 待機 → 攻撃、歩き → ジャンプなどの遷移

## API

| 型 | 役割 |
|----|------|
| `MotionCrossfade` | primary / secondary クリップとクロスフェード時間 |
| `CmpMotionCrossfade` | Entity の `TransformLH` を更新 |

```cpp
auto *motion = entity->AddUpdCmp<CmpMotionCrossfade>();
motion->PlayClip("../EditorAssets/Motion/sample_motion_a.json");
// 後から
motion->CrossfadeToClip("../EditorAssets/Motion/sample_motion_b.json", 0.45f);
```

補間曲線は `Math::Ease::InOutQuad`（`BlendMotionTransforms` と組み合わせ）。

## 関連

- ブレンド重み固定: `CmpMotionBlend`
- 仕様: `motion_blending.md`
